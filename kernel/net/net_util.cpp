#include "net_util.hpp"

#include <string.h>

namespace net {
  uint16_t hton16(uint16_t x_host) {
    uint8_t *p = (uint8_t *)&x_host;
    return (uint16_t)p[0] << 8 | p[1];
  }

  uint16_t hton32(uint32_t x_host) {
    uint8_t *p = (uint8_t *)&x_host;
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | 
           (uint32_t)p[2] << 8  | (uint32_t)p[3];
  }

  void macaddr_copy(macaddr_t dest, const macaddr_t src) {
    memcpy(dest, src, MACADDR_LEN);
  }

  void ipaddr_copy(ipaddr_t dest, const ipaddr_t src) {
    memcpy(dest, src, IPADDR_LEN);
  }

  bool same_macaddr(macaddr_t mac1, macaddr_t mac2) {
    return mac1[0] == mac2[0] &&
           mac1[1] == mac2[1] &&
           mac1[2] == mac2[2] &&
           mac1[3] == mac2[3] &&
           mac1[4] == mac2[4] &&
           mac1[5] == mac2[5];
  }
  
  bool same_ipaddr(ipaddr_t ip1, ipaddr_t ip2) {
    return ip1[0] == ip2[0] &&
           ip1[1] == ip2[1] &&
           ip1[2] == ip2[2] &&
           ip1[3] == ip2[3];
  }
}
