#pragma once

#include <cstdint>
#include "packet.hpp"
#include "mbuf.hpp"
#include "net_util.hpp"

namespace net {
  struct ethernet_header {
    macaddr_t dest_address;
    macaddr_t src_address;
    uint16_t type;
  } __attribute__((packed));

  void send_ethernet(mbuf *payload);
}
