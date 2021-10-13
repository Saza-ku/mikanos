#include "icmp.hpp"

#include "checksum.hpp"
#include "mbuf.hpp"
#include "ip.hpp"
#include "logger.hpp"

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

    Log(kError, "ICMP: Send ICMP echo request to %d.%d.%d.%d\n", dst[0], dst[1], dst[2], dst[3]);

    send_ip(dst, PROTO_ICMP, packet);
  }

  void receive_icmp(mbuf *mbuf, ipaddr_t src) {
    icmp_header header;
    mbuf->read(&header, sizeof(header));

    switch (header.type) {
      case ICMP_TYPE_ECHO_REPLY:
        receive_icmp_echo_reply(src);
        break;
      default:
        Log(kError, "ICMP: Received unknown packet: type=%x\n", header.type);
        break;
    }
  }

  void receive_icmp_echo_reply(ipaddr_t src) {
    Log(kError, "ICMP: Received ICMP echo reply from %d.%d.%d.%d\n", src[0], src[1], src[2], src[3]);
  }
}
