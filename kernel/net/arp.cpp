#include "arp.hpp"

#include "ethernet.hpp"
#include "mbuf.hpp"
#include "net_util.hpp"
#include "logger.hpp"

namespace net {
  void send_arp(ipaddr_t dest_ip) {
    arp_packet packet;
    ipaddr_copy(packet.src_ip, (ipaddr_t){10, 0, 2, 15});
    macaddr_copy(packet.src_mac, (macaddr_t){0x52, 0x54, 0x0, 0x12, 0x34, 0x56});
    ipaddr_copy(packet.dest_ip, dest_ip);
    macaddr_copy(packet.dest_mac, (macaddr_t){0, 0, 0, 0, 0, 0});
    packet.opcode = hton16(1);

    mbuf *packet_mbuf = new mbuf(&packet, sizeof(packet));
    send_ethernet(packet_mbuf, TYPE_ARP);
  }

  void receive_arp(mbuf *mbuf) {
    arp_packet *packet = nullptr;
    size_t len = mbuf->read(packet, sizeof(*packet));
    if (len != sizeof(*packet)) {
      Log(kError, "ARP: Received invalid packet\n");
    }

    switch(ntoh16(packet->opcode)) {
      case 2:
        Log(kError, "ARP: Received reply\n");
        Log(kError, "%d.%d.%d.%d - %x:%x:%x:%x:%x:%x\n",
            packet->src_ip[0],
            packet->src_ip[1],
            packet->src_ip[2],
            packet->src_ip[3],
            packet->src_mac[0],
            packet->src_mac[1],
            packet->src_mac[2],
            packet->src_mac[3],
            packet->src_mac[4],
            packet->src_mac[5]
        );
        break;
      default:
        Log(kError, "ARP: Received unknown packet\n");
        break;
    }
  }
}
