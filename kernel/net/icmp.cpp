#include "icmp.hpp"

#include "checksum.hpp"
#include "mbuf.hpp"
#include "ip.hpp"

namespace net {
  void send_icmp_echo_request(ipaddr_t dst) {
    struct icmp_header header;
    header.type = ICMP_TYPE_ECHO_REQUEST;
    header.code = 0;
    header.id = 0;
    header.checksum = 0;

    char payload[] = "やったね";

    // Compute checksum.
    checksum_t checksum;
    checksum_init(&checksum);
    checksum_update(&checksum, &header, sizeof(header));
    checksum_update(&checksum, payload, sizeof(payload));
    header.checksum = checksum_finish(&checksum);

    mbuf *packet = new mbuf(&header, sizeof(header));
    packet->append_bytes(payload, sizeof(payload));

    send_ip(dst, PROTO_ICMP, packet);
  }
}
