#pragma once

#include <cstdint>
#include "net_util.hpp"

namespace net {
  struct arp_packet {
    uint16_t hw_type = hton16(1);
    uint16_t proto_type = hton16(0x0800);
    uint8_t hw_len = 6;
    uint8_t proto_len = 4;
    uint16_t opcode;
    macaddr_t src_mac;
    ipaddr_t src_ip;
    macaddr_t dest_mac;
    ipaddr_t dest_ip;
  } __attribute__((packed));

  void send_arp(ipaddr_t dest_ip);
}
