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

    ipaddr_t next_router;
    if(same_ipaddr(ip, IPV4_ADDR_BROADCAST)) {
      ipaddr_copy(next_router, ip);
    }
    else {
      ipaddr_copy(next_router, ipaddr_t{10, 0, 2, 2});
    }

    if (!arp_resolve(next_router, dest_mac)) {
      arp_enqueue(payload, type, next_router);
      send_arp(next_router);
      return;
    }
    macaddr_copy(header.dest_address, dest_mac);
    macaddr_copy(header.src_address, e1000::nic->macaddr);
    header.type = hton16(type);
    mbuf *header_buf = new mbuf(&header, sizeof(header));
    header_buf->append(payload);

    e1000::nic->Send(header_buf);
  }

  void receive_ethernet(mbuf *buf) {
    ethernet_header header;
    size_t len = buf->read(&header, sizeof(header));
    if (len != sizeof(header)) {
      Log(kError, "Ethernet: Received invalid Packet\n");
      return;
    }

    uint16_t type = ntoh16(header.type);

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
