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
}
