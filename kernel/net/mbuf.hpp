#pragma once
#include <cstdint>
#include <cstddef>

#define MBUF_MAX_LEN 2048

namespace net {
  class mbuf {
   public:
    mbuf(const void *data, size_t len);
    size_t read(void *buf, size_t buf_len);
    uint8_t *data();
    void prepend(mbuf *new_head);
    void append(mbuf *new_tale);

   private:
    mbuf *next_;
    uint16_t offset_;
    uint16_t offset_end_;
    uint8_t data_[MBUF_MAX_LEN];
  };
}