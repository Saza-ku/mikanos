#include "ethernet.hpp"

#include <cstddef>
#include <cstdint>

#include "mbuf.hpp"
#include "net_util.hpp"
#include "nic/e1000.hpp"
#include "arp.hpp"
#include "logger.hpp"

namespace net {
  void send_ethernet(mbuf *payload, uint16_t type, ipaddr_t ip) {
    ethernet_header header;
    macaddr_t dest_mac;
    if (!arp_resolve(ip, dest_mac)) {
      arp_enqueue(payload, type, ip);
      send_arp(ip);
      return;
    }
    macaddr_copy(header.dest_address, MACADDR_BROADCAST);
    macaddr_copy(header.src_address, dest_mac);
    header.type = hton16(type);
    mbuf *header_buf = new mbuf(&header, sizeof(header));
    header_buf->append(payload);

    e1000::nic->Send(header_buf);
  }

  void receive_ethernet(mbuf *buf) {
    ethernet_header *header = nullptr;
    size_t len = buf->read(header, sizeof(*header));
    if (len != sizeof(*header)) {
      Log(kError, "Ethernet: Received invalid Packet\n");
      return;
    }

    uint16_t type = ntoh16(header->type);

    switch (type)
    {
    case TYPE_ARP:
    Log(kError, "Ethernet: Received ARP\n");
      receive_arp(buf);
      break;
    
    default:
      Log(kError, "Ethernet: Received unknown packet : type=%04x\n", type);
      break;
    }
  }
}
