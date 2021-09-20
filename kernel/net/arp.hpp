#pragma once

#include <cstdint>

#include "net_util.hpp"
#include "mbuf.hpp"

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

  struct mbuf_list {
    mbuf *buf;
    mbuf_list *next = nullptr;
    uint16_t packet_type;
  };
  
  class arp_entry {
   public:
    macaddr_t mac;
    ipaddr_t ip;
    bool resolved = false;
    bool in_use = false;
    void send_packets();
    void add_packet(mbuf *packet, uint16_t type);
   private:
    mbuf_list *packets_ptr = nullptr;
  };

  void send_arp(ipaddr_t dest_ip);
  void receive_arp(mbuf *mbuf);
  bool arp_resolve(ipaddr_t ip, macaddr_t mac);
  void arp_enqueue(mbuf *packet, uint16_t type, ipaddr_t ip);
}
