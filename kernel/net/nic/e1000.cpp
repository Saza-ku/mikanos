#include "net/nic/e1000.hpp"
#include "pci.hpp"
#include "logger.hpp"

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

  void Nic::Initialize() {
    // ディスクリプタリングのメモリを確保
    static t_descriptor desc_ring_buf[sizeof(t_descriptor) * T_DESC_NUM]
      __attribute__((aligned(16)));
    desc_ring_addr_ = (t_descriptor *)desc_ring_buf;

    // 各ディスクリプタのフィールドを初期化する
    for (int i = 0; i < T_DESC_NUM; i++) {
      desc_ring_addr_[i].buffer_address = 0;
      desc_ring_addr_[i].length = 0;
      desc_ring_addr_[i].cso = 0;
      desc_ring_addr_[i].cmd = T_DESC_CMD_RS;
      desc_ring_addr_[i].sta = 0;
      desc_ring_addr_[i].rsv = 0;
      desc_ring_addr_[i].css = 0;
      desc_ring_addr_[i].special = 0;
    }

    tale_ = 0;

    // NIC の初期化処理
    SetNicReg(CTRL_OFFSET,
      CTRL_FD | CTRL_ASDE | CTRL_SLU | CTRL_SPEED | CTRL_FRCPED | CTRL_FRCDPLX);

    // 送信処理の設定
    SetNicReg(TCTL_OFFSET,
      TCTL_EN | TCTL_PSP | TCTL_CT | TCTL_COLD);
    SetNicReg(TIPG_OFFSET,
      TIPG_IPGT | TIPG_IPGR1 | TIPG_IPGR2);
    SetNicReg(TDBAL_OFFSET, (uintptr_t)desc_ring_addr_ & static_cast<uint64_t>(0xffffffff));
    SetNicReg(TDBAH_OFFSET, (uintptr_t)desc_ring_addr_ >> 32);
    SetNicReg(TDLEN_OFFSET, sizeof(t_descriptor) * T_DESC_NUM);
    SetNicReg(TDH_OFFSET, tale_);
    SetNicReg(TDT_OFFSET, tale_);
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

    // TODO: 割り込みのなんか (MSI)

    // BAR を読む
    const WithError<uint64_t> nic_bar = pci::ReadBar(*nic_dev, 0);
    Log(kDebug, "ReadBar: %s\n", nic_bar.error.Name());
    const uint64_t nic_mmio_base = nic_bar.value & ~static_cast<uint64_t>(0xf);
    Log(kDebug, "NIC mmio_base = %08lx\n", nic_mmio_base);

    nic = new Nic{nic_mmio_base};
    nic->Initialize();
  }
}
