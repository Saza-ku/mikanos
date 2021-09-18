#include "ethernet.hpp"

#include <cstddef>
#include <cstdint>

#include "mbuf.hpp"
#include "net_util.hpp"
#include "nic/e1000.hpp"
#include "arp.hpp"
#include "logger.hpp"

namespace net {
  void send_ethernet(mbuf *payload, uint16_t type) {
    ethernet_header header;
    macaddr_copy(header.dest_address, MACADDR_BROADCAST);
    // ハードコーディングやめたい
    macaddr_copy(header.src_address, (macaddr_t){0x52, 0x54, 0x0, 0x12, 0x34, 0x56});
    header.type = hton16(type);
    mbuf *header_buf = new mbuf(&header, sizeof(header));
    header_buf->append(payload);
    uint8_t buffer[PACKET_SIZE];
    size_t len = header_buf->read(buffer, PACKET_SIZE);

    e1000::nic->Send(buffer, len);
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
