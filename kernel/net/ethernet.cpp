#include "ethernet.hpp"
#include "mbuf.hpp"
#include "net_util.hpp"
#include "nic/e1000.hpp"
#include <cstddef>
#include <cstdint>

namespace net {
  void send_ethernet(mbuf *payload) {
    ethernet_header header;
    macaddr_copy(header.dest_address, MACADDR_BROADCAST);
    // ハードコーディングやめたい
    macaddr_copy(header.src_address, (macaddr_t){0x52, 0x54, 0x0, 0x12, 0x34, 0x56});
    header.type = hton16(0x0806);
    mbuf *header_buf = new mbuf(&header, sizeof(header));
    header_buf->append(payload);
    uint8_t buffer[PACKET_SIZE];
    size_t len = header_buf->read(buffer, PACKET_SIZE);

    e1000::nic->Send(buffer, len);
  }
}
