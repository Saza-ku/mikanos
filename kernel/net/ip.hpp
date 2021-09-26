#pragma once

#include <cstdint>

#include "net_util.hpp"
#include "mbuf.hpp"

namespace net {
  #define DEFAULT_TTL 64
  #define PROTO_ICMP 1

  struct ip_header {
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t length;
    uint16_t id;
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t checksum;
    ipaddr_t src_addr;
    ipaddr_t dst_addr;
  } __attribute__((packed));

  void send_ip(ipaddr_t dst, uint8_t proto, mbuf *payload);
}
