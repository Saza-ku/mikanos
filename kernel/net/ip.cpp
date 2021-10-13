#include "ip.hpp"

#include "icmp.hpp"
#include "checksum.hpp"
#include "ethernet.hpp"
#include "net/nic/e1000.hpp"
#include "logger.hpp"

namespace net {
  void send_ip(ipaddr_t dst, uint8_t proto, mbuf *payload) {
    ip_header header;
    header.version_ihl = 0x45;
    // TODO: 調べる
    header.type_of_service = 0;
    header.length = hton16(sizeof(header) + payload->length());
    header.id = 0;
    header.flags_offset = 0;
    header.ttl = DEFAULT_TTL;
    header.proto = proto;
    header.checksum = 0;
    ipaddr_copy(header.dst_addr, dst);
    ipaddr_copy(header.src_addr, e1000::nic->ipaddr);

    checksum_t checksum;
    checksum_init(&checksum);
    checksum_update(&checksum, &header, sizeof(header));
    header.checksum = checksum_finish(&checksum);

    mbuf *ip_packet = new mbuf(&header, sizeof(header));
    ip_packet->append(payload);

    send_ethernet(ip_packet, TYPE_IP, dst);
  }

  void receive_ip(mbuf *mbuf) {
    ip_header header;
    mbuf->read(&header, sizeof(header));

    switch (header.proto){
      case PROTO_ICMP:
        Log(kError, "IP: Received ICMP\n");
        receive_icmp(mbuf, header.src_addr);
        break;
      default:
        Log(kError, "IP: Received unknown packet: proto=%x\n", header.proto);
        break;
    }
  }
}
