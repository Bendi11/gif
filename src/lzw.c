#include "gif/lzw.h"


uint16_t bitbuffer_readn(bitbuffer_t *buf, uint8_t width) {
    uint16_t val = 0;
    if(buf->bitpos < width) {
        uint16_t val;
        uint8_t bits_left = width - buf->bitpos;
        val |= (buf->buf[buf->idx] ^ (0b11111110 << buf->bitpos)) << bits_left;
        buf->idx += 1;
        buf->bitpos = 7;
        width = bits_left;
    }

    uint8_t shift = buf->bitpos - (width - 1);
    val |= (buf->buf[buf->idx] >> shift) ^ (0xff << width);
    buf->bitpos -= width;
    return val;
}

void bitbuffer_new(bitbuffer_t *buf, void *bytes) {
    buf->buf = (uint8_t*)bytes;
    buf->idx = 0;
    buf->bitpos = 7;
}
