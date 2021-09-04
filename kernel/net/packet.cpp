#include <string.h>
#include "packet.hpp"

namespace net {
  Packet::Packet(void *buf, uint32_t length) : buf_{buf}, length_{length} {}
  void Packet::Dump(LogLevel level) {
    uint32_t buffer[length_ / 4];
    memcpy(buffer, buf_, length_);
    Log(level, "DUMP: %x\n", buffer);
  }
}
