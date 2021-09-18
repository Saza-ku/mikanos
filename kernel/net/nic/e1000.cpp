#include "e1000.hpp"

#include <string.h>

#include "pci.hpp"
#include "logger.hpp"
#include "interrupt.hpp"
#include "net/mbuf.hpp"
#include "net/ethernet.hpp"

namespace net::e1000 {
  Nic *nic;

  Nic::Nic(uintptr_t mmio_base) : mmio_base_{mmio_base} {}

  uint32_t Nic::GetNicReg(uint16_t reg_offset) {
    uintptr_t reg_addr = reg_offset + mmio_base_;
    return *(uint32_t *)reg_addr;
  }

  void Nic::SetNicReg(uint16_t reg_offset, uint32_t value) {
    uintptr_t reg_addr = (uintptr_t)(reg_offset + mmio_base_);
    *(uint32_t *)reg_addr = value;
  }

  void Nic::Initialize(bool accept_all) {
    // 送信ディスクリプタリングのメモリを確保
    static t_descriptor t_desc_ring_buf[sizeof(t_descriptor) * T_DESC_NUM]
      __attribute__((aligned(16)));
    t_desc_ring_addr_ = (t_descriptor *)t_desc_ring_buf;

    // 受信ディスクリプタのメモリを確保
    static r_descriptor r_desc_ring_buf[sizeof(r_descriptor) * R_DESC_NUM]
      __attribute__((aligned(16)));
    r_desc_ring_addr_ = (r_descriptor *)r_desc_ring_buf;

    // 送信ディスクリプタのフィールドを初期化する
    for (int i = 0; i < T_DESC_NUM; i++) {
      t_desc_ring_addr_[i].buffer_address = 0;
      t_desc_ring_addr_[i].length = 0;
      t_desc_ring_addr_[i].checksum_offset = 0;
      t_desc_ring_addr_[i].command = T_DESC_CMD_RS;
      t_desc_ring_addr_[i].status = 0;
      t_desc_ring_addr_[i].reserved = 0;
      t_desc_ring_addr_[i].checksum_start_field = 0;
      t_desc_ring_addr_[i].special = 0;
    }

    static uint8_t packet_buffer[R_DESC_NUM][PACKET_SIZE];
    // 受信ディスクリプタのフィールドを初期化する
    for (int i = 0; i < R_DESC_NUM; i++) {
      r_desc_ring_addr_[i].buffer_address =
        (uint64_t)&packet_buffer[i];
      r_desc_ring_addr_[i].status = 0;
      r_desc_ring_addr_[i].errors = 0;
    }

    t_tale_ = 0;
    r_tale_ = R_DESC_NUM - 1;

    // NIC の初期化処理
    SetNicReg(CTRL_OFFSET,
      CTRL_FD | CTRL_ASDE | CTRL_SLU | CTRL_SPEED | CTRL_FRCPED | CTRL_FRCDPLX);

    // 送信処理の設定
    uint32_t tctl_value = TCTL_EN | TCTL_PSP | TCTL_CT | TCTL_COLD;
    SetNicReg(TCTL_OFFSET, tctl_value);
    SetNicReg(TIPG_OFFSET, TIPG_IPGT | TIPG_IPGR1 | TIPG_IPGR2);
    SetNicReg(TDBAL_OFFSET, (uintptr_t)t_desc_ring_addr_ & static_cast<uint64_t>(0xffffffff));
    SetNicReg(TDBAH_OFFSET, (uintptr_t)t_desc_ring_addr_ >> 32);
    SetNicReg(TDLEN_OFFSET, sizeof(t_descriptor) * T_DESC_NUM);
    SetNicReg(TDH_OFFSET, t_tale_);
    SetNicReg(TDT_OFFSET, t_tale_);

    // 受信処理の設定
    uint32_t rctl_value = RCTL_BAM | RCTL_EN;
    if (accept_all) {
      rctl_value = rctl_value | RCTL_SBP | RCTL_UPE | RCTL_MPE;
    }
    uint32_t ims_value = IMS_LSC | IMS_RXSEQ | IMS_RXDMT0 | IMS_RXO
                       | IMS_RXT0;
    SetNicReg(RCTL_OFFSET, rctl_value);
    SetNicReg(IMS_OFFSET, ims_value);
    SetNicReg(RDTR_OFFSET, RDTR_DELAY);
    SetNicReg(RADV_OFFSET, RADV_DELAY);
    SetNicReg(RDBAL_OFFSET, (uintptr_t)r_desc_ring_addr_ & static_cast<uint64_t>(0xffffffff));
    SetNicReg(RDBAH_OFFSET, (uintptr_t)r_desc_ring_addr_ >> 32);
    SetNicReg(RDLEN_OFFSET, sizeof(t_descriptor) * R_DESC_NUM);
    SetNicReg(RDH_OFFSET, 0);
    SetNicReg(RDT_OFFSET, r_tale_);
  }

  void Initialize() {
    pci::Device* nic_dev = nullptr;
    for (int i = 0; i < pci::num_device; ++i) {
      if (pci::devices[i].class_code.Match(0x02u, 0x00u, 0x00u)) {
        nic_dev = &pci::devices[i];
        if (0x8086 == pci::ReadVendorId(*nic_dev)) {
          break;
        }
      }
    }

    if (nic_dev) {
      Log(kInfo, "NIC has been found: %d.%d.%d\n",
          nic_dev->bus, nic_dev->device, nic_dev->function);
    } else {
      Log(kError, "NIC has not been found\n");
      exit(1);
    }

    const uint8_t bsp_local_apic_id =
      *reinterpret_cast<const uint32_t*>(0xfee00020) >> 24;
    Error err = pci::ConfigureMSIFixedDestination(
        *nic_dev, bsp_local_apic_id,
        pci::MSITriggerMode::kLevel, pci::MSIDeliveryMode::kFixed,
        InterruptVector::kNic, 0);
    if (err) {
      Log(kError, "Error while NIC MSI configuration: %s:%d\n", err.File(), err.Line());
    }

    // BAR を読む
    const WithError<uint64_t> nic_bar = pci::ReadBar(*nic_dev, 0);
    Log(kDebug, "ReadBar: %s\n", nic_bar.error.Name());
    const uint64_t nic_mmio_base = nic_bar.value & ~static_cast<uint64_t>(0xf);
    Log(kDebug, "NIC mmio_base = %08lx\n", nic_mmio_base);

    nic = new Nic{nic_mmio_base};
    nic->Initialize(false);
  }

  uint8_t Nic::Send(void *buf, uint16_t length) {
    // ディスクリプタリングを書き換える
    t_descriptor *desc = &this->t_desc_ring_addr_[this->t_tale_];
    desc->buffer_address = (uintptr_t)buf;
    desc->length = length;
    desc->command = desc->command | T_DESC_CMD_EOP;
    desc->status = 0;

    // NIC の TALE をインクリメントする
    this->t_tale_++;
    SetNicReg(TDT_OFFSET, this->t_tale_);

    // 送信処理の完了を待つ
    uint8_t send_status = 0;
    while(!send_status) {
      send_status = desc->status & 0x0fu;
    }
    return send_status;
  }

    void Nic::Receive () {
    uint32_t next_tale = (r_tale_ + 1) % R_DESC_NUM;
    r_descriptor *desc = &r_desc_ring_addr_[next_tale];
    uint16_t len = 0;

    if (desc->status != 0) {
      mbuf *buf = new mbuf((void *)desc->buffer_address, desc->length);

      r_tale_ = next_tale;
      SetNicReg(RDT_OFFSET, r_tale_);
      receive_ethernet(buf);
    }
  }

  bool Nic::HasPacket() {
    return GetNicReg(RDH_OFFSET) != (r_tale_ + R_DESC_NUM + 1) % R_DESC_NUM;
  }

  void Nic::AckInterrupt() {
    SetNicReg(ICR_OFFSET, IMS_LSC | IMS_RXDMT0 | IMS_RXO | IMS_RXSEQ | IMS_RXT0);
  }
}
