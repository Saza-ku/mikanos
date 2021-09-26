#include "checksum.hpp"

namespace net {
  void checksum_init(checksum_t *c) {
    *c = 0;
  }

  void checksum_update(checksum_t *c, const void *data, size_t len) {
    const uint16_t *words = (uint16_t *)data;
    for (size_t i = 0; i < len / 2; i++) {
      *c += words[i];
    }
    if (len % 2 != 0) {
      *c += ((uint8_t *)data)[len - 1];
    }
  }

  uint32_t checksum_finish(checksum_t *c) {
    *c = (*c >> 16) + (*c & 0xffff);
    *c += *c >> 16;
    return ~*c;
  }
}