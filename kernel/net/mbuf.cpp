#include "mbuf.hpp"
#include <algorithm>
#include <cstddef>

namespace net {
  mbuf::mbuf(const void *data, size_t len) {
    const uint8_t *p = (uint8_t *)data;
    size_t mbuf_len = std::min(len, (size_t)MBUF_MAX_LEN);
    offset_ = 0;
    offset_end_ = mbuf_len;
    memcpy(data_, p, mbuf_len);
    p += mbuf_len;
    next_ = NULL;
    if (MBUF_MAX_LEN < len) {
      next_ = new mbuf(p, len - mbuf_len);
    }
  }

  size_t mbuf::read(void *buf_, size_t buf_len) {
    uint8_t *buf = (uint8_t *)buf_;
    size_t read_len = 0;
    mbuf *mbuf_ = this;
    while (true) {
      size_t mbuf_len = mbuf_->offset_end_ - mbuf_->offset_;
      if (!mbuf_len && mbuf_->next_) {
        mbuf *prev = mbuf_;
        mbuf_ = mbuf_->next_;
        delete prev;
        continue;
      }

      size_t copy_len = std::min(buf_len - read_len, mbuf_len);
      if (copy_len <= 0) {
        break;
      }

      memcpy(&buf[read_len], mbuf_->data(), copy_len);
      mbuf_->offset_ += copy_len;
      read_len += copy_len;
    }

    return read_len;
  }

  uint8_t *mbuf::data() {
    return &data_[offset_];
  }

  void mbuf::append(mbuf *new_tale) {
    mbuf *old_tale = this;
    while(old_tale->next_) {
      old_tale = old_tale->next_;
    }
    old_tale->next_ = new_tale;
  }

  void mbuf::prepend(mbuf *new_head) {
    new_head->next_ = this;
  }
}