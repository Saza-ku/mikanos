#pragma once

#include <cstdint>

#define ALIGN_MARGIN   15
#define T_DESC_NUM     8

#define CTRL_OFFSET    0x00000u
#define TDBAL_OFFSET   0x03800u
#define TDBAH_OFFSET   0x03804u
#define TDLEN_OFFSET   0x03808u
#define TDH_OFFSET     0x03810u
#define TDT_OFFSET     0x03818u
#define TCTL_OFFSET    0x00400u
#define TIPG_OFFSET    0x00410u

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
    uint8_t  cso;     // checksum offset
    uint8_t  cmd;     // command
    uint8_t  sta : 4; // status
    uint8_t  rsv : 4; // reserved
    uint8_t  css;     // checksum start field
    uint16_t special;
  } __attribute__((packed));

  class Nic {
   public:
    Nic(uintptr_t mmio_base);
    void Initialize();
    uint8_t Send(void *buf, uint16_t length);
   
   private:
    const uintptr_t mmio_base_;
    uint32_t tale_;
    t_descriptor *desc_ring_addr_;
    uint32_t GetNicReg(uint16_t reg_offset);
    void SetNicReg(uint16_t reg_offset, uint32_t value);
  };

  void Initialize();

  extern Nic *nic;
}
