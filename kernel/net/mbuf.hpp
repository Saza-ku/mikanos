#pragma once

#include <cstdint>
#include <cstddef>

#define MBUF_MAX_LEN 512

namespace net {
  class mbuf {
   public:
    mbuf(const void *data, size_t len);
    size_t read(void *buf, size_t buf_len);
    size_t length();
    uint8_t *data();
    void prepend(mbuf *new_head);
    void append(mbuf *new_tale);
    void append_bytes(void *data, size_t len);

   private:
    mbuf *next_;
    uint16_t offset_;
    uint16_t offset_end_;
    uint8_t data_[MBUF_MAX_LEN];
    size_t length_one();
  };
}
