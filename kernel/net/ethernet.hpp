#pragma once

#include <cstdint>

#include "mbuf.hpp"
#include "net_util.hpp"

#define TYPE_ARP 0x0806

namespace net {
  struct ethernet_header {
    macaddr_t dest_address;
    macaddr_t src_address;
    uint16_t type;
  } __attribute__((packed));

  void send_ethernet(mbuf *payload, uint16_t type, ipaddr_t dest_ip);
  void receive_ethernet(mbuf *buf);
}
