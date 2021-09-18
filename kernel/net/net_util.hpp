#include <cstdint>
#include <string.h>

#define MACADDR_LEN 6
#define IPADDR_LEN 4
#define MACADDR_BROADCAST ((macaddr_t){0xff, 0xff, 0xff, 0xff, 0xff, 0xff})

typedef uint8_t macaddr_t[MACADDR_LEN];
typedef uint8_t ipaddr_t[IPADDR_LEN];

namespace net {
  uint16_t hton16(uint16_t x_host);
  uint16_t hton32(uint32_t x_host);
  #define ntoh16(x) hton16(x)
  #define ntoh32(x) hton32(x)
  void macaddr_copy(macaddr_t dest, const macaddr_t src);
  void ipaddr_copy(ipaddr_t dest, const ipaddr_t src);
}
