#include <cstdint>

#include "net/net_util.hpp"
#include "mbuf.hpp"

namespace net {
  #define ICMP_TYPE_ECHO_REPLY   0x00
  #define ICMP_TYPE_ECHO_REQUEST 0x08

  struct icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq_no;
  } __attribute__((packed));

  void send_icmp_echo_request(ipaddr_t dst);
  void receive_icmp(mbuf *mbuf, ipaddr_t src);
  void receive_icmp_echo_reply(ipaddr_t src);
}
