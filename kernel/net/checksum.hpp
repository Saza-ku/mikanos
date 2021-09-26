#pragma once

#include <cstdint>
#include <cstddef>

namespace net {
  typedef uint32_t checksum_t;

  void checksum_init(checksum_t *c);
  void checksum_update(checksum_t *c, const void *data, size_t len);
  uint32_t checksum_finish(checksum_t *c);
}