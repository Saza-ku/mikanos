#pragma once

#include <cstdint>
#include "logger.hpp"

namespace net {
  class Packet {
   public:
    Packet(void *buf, uint32_t length);
    void Dump(LogLevel level);
   private:
    void *buf_;
    uint32_t length_;
  };
}
