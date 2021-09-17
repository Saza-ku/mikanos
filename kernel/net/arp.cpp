#include "arp.hpp"
#include "ethernet.hpp"
#include "mbuf.hpp"
#include "net_util.hpp"

namespace net {
  void send_arp(ipaddr_t dest_ip) {
    arp_packet packet;
    ipaddr_copy(packet.src_ip, (ipaddr_t){10, 0, 2, 15});
    macaddr_copy(packet.src_mac, (macaddr_t){0x52, 0x54, 0x0, 0x12, 0x34, 0x56});
    ipaddr_copy(packet.dest_ip, dest_ip);
    macaddr_copy(packet.dest_mac, (macaddr_t){0, 0, 0, 0, 0, 0});
    packet.opcode = hton16(1);

    mbuf *packet_mbuf = new mbuf(&packet, sizeof(packet));
    send_ethernet(packet_mbuf);
  }
}
