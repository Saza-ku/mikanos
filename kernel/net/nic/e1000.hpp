#pragma once

#include <cstdint>

#define ALIGN_MARGIN   15
#define T_DESC_NUM     8
#define R_DESC_NUM     16
#define PACKET_SIZE    2048

#define CTRL_OFFSET    0x00000u
#define ICR_OFFSET     0x000c0u
#define IMS_OFFSET     0x000d0u
#define TCTL_OFFSET    0x00400u
#define TIPG_OFFSET    0x00410u
#define TDBAL_OFFSET   0x03800u
#define TDBAH_OFFSET   0x03804u
#define TDLEN_OFFSET   0x03808u
#define TDH_OFFSET     0x03810u
#define TDT_OFFSET     0x03818u
#define RCTL_OFFSET    0x00100u
#define RDBAL_OFFSET   0x02800u
#define RDBAH_OFFSET   0x02804u
#define RDLEN_OFFSET   0x02808u
#define RDH_OFFSET     0x02810u
#define RDT_OFFSET     0x02818u
#define RDTR_OFFSET    0x02820u
#define RADV_OFFSET    0x0282Cu

#define CTRL_FD        0x00000001u // CTRL[0]
#define CTRL_ASDE      0x00000020u // CTRL[5]
#define CTRL_SLU       0x00000040u // CTRL[6]
#define CTRL_SPEED     0x00000200u // CTRL[9:8] = 10b
#define CTRL_FRCPED    0x00000800u // CTRL[11]
#define CTRL_FRCDPLX   0x00001000u // CTRL[12]

#define TCTL_EN        0x00000002u // TCTL[1]
#define TCTL_PSP       0x00000008u // TCTL[3]
#define TCTL_CT        0x00000100u // TCTL[11:4] = 10h
#define TCTL_COLD      0x00400000u // TCTL[21:12] = 40h

#define RCTL_EN        0x00000002u // RCTL[1]
#define RCTL_SBP       0x00000004u // RCTL[2]
#define RCTL_UPE       0x00000008u // RCTL[3]
#define RCTL_MPE       0x00000010u // TCTL[4]
#define RCTL_BAM       0x00008000u // RCTL[15]

#define IMS_LSC        0x00000004u // MSI[2]
#define IMS_RXSEQ      0x00000008u // MSI[3]
#define IMS_RXDMT0     0x00000010u // MSI[4]
#define IMS_RXO        0x00000040u // MSI[6]
#define IMS_RXT0       0x00000080u // MSI[7]

#define RDTR_DELAY     0x00001000u // Delay Timer = 16^3
#define RADV_DELAY     0x00001000u

#define TIPG_IPGT      8u
#define TIPG_IPGR1     8u
#define TIPG_IPGR2     6u

#define T_DESC_CMD_RS  0b00001000u
#define T_DESC_CMD_EOP 0b00000001u

namespace net::e1000 {
  /**
   * @struct  t_descriptor
   * @brief   送信用ディスクリプタ
   * 
   * 送信用ディスクリプタには 3 種類あるが、
   * ここでは Legacy Descriptor として実装
   */
  struct t_descriptor {
    uint64_t buffer_address;
    uint16_t length;
    uint8_t  checksum_offset;
    uint8_t  command;
    uint8_t  status : 4;
    uint8_t  reserved : 4;
    uint8_t  checksum_start_field;
    uint16_t special;
  } __attribute__((packed));

  /** 
   * @struct r_descriptor
   * @brief  受信用ディスクリプタ
  */
  struct r_descriptor {
    uint64_t buffer_address;
    uint16_t length;
    uint16_t reserved;
    uint8_t  status;
    uint8_t  errors;
    uint16_t special;
  } __attribute__((packed));

  class Nic {
   public:
    Nic(uintptr_t mmio_base);
    void Initialize(bool accept_all);
    uint8_t Send(void *buf, uint16_t length);
    void Receive();
    bool HasPacket();
    void AckInterrupt();
    
   private:
    const uintptr_t mmio_base_;
    uint32_t t_tale_;
    uint32_t r_tale_;
    t_descriptor *t_desc_ring_addr_;
    r_descriptor *r_desc_ring_addr_;
    void SetNicReg(uint16_t reg_offset, uint32_t value);
    uint32_t GetNicReg(uint16_t reg_offset);
  };

  void Initialize();

  extern Nic *nic;
}
