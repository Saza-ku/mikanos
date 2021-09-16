#include "ethernet.hpp"
#include "mbuf.hpp"
#include "nic/e1000.hpp"
#include <cstddef>

namespace {
  // TODO: こんなんやめたい
  uint64_t make_mac_addr(uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6) {
    uint64_t mac_addr = 0;
    mac_addr += addr6;
    mac_addr += addr5 * (1<<8);
    mac_addr += addr4 * (1<<16);
    mac_addr += addr3 * ((uint64_t)1<<24);
    mac_addr += addr2 * ((uint64_t)1<<32);
    mac_addr += addr1 * ((uint64_t)1<<40);
    return mac_addr;
  }
}

namespace net {
  void send_ethernet(mbuf *payload) {
    ethernet_header header;
    header.dest_address = 1;
    // ハードコーディングやめたい
    header.src_address = make_mac_addr(52,54,0,12,34,56);
    mbuf *header_buf = new mbuf(&header, sizeof(header));
    header_buf->append(payload);
    uint8_t buffer[PACKET_SIZE];
    size_t len = header_buf->read(buffer, PACKET_SIZE);

    e1000::nic->Send(buffer, len);
  }
}