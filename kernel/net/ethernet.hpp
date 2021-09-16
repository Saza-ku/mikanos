#pragma once

#include <cstdint>
#include "packet.hpp"

namespace net {
  struct ethernet_header {
    uint64_t preamble = 0xaaaaaaaaaaaaaaab;
    uint64_t dest_address : 48;
    uint64_t src_address  : 48;
    uint16_t type;
  } __attribute__((packed));
}
