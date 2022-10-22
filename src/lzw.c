#include "gif/lzw.h"
#include "gif/buf.h"
#include "gif/error.h"


uint16_t bitbuffer_readn(bitbuffer_t *buf, uint8_t width) {
    uint16_t val = 0;
    if(buf->bitpos < width) {
        uint8_t bits_left = width - (buf->bitpos + 1);
        val |= (buf->buf[buf->idx] & ~(0b11111110 << buf->bitpos)) << bits_left;
        buf->idx += 1;
        buf->bitpos = 7;
        width = bits_left;
    }

    if(width != 0) {
        uint8_t shift = buf->bitpos - (width - 1);
        val |= (buf->buf[buf->idx] >> shift) & ~(0xff << width);
        buf->bitpos -= width;
    }
    return val;
}

void bitbuffer_new(bitbuffer_t *buf, void *bytes) {
    buf->buf = (uint8_t*)bytes;
    buf->idx = 0;
    buf->bitpos = 7;
}

gif_err_t lzw_decompressor_new(lzw_decompressor_t *decom, uint8_t min_code_width) {
    return lzw_decompressor_new_with_cap(decom, 1024, min_code_width);
}

gif_err_t lzw_decompressor_new_with_cap(lzw_decompressor_t *decom, uint32_t cap, uint8_t min_code_width) {
    gif_err_t res;
    if((res = bytebuf_new_with_cap(&decom->output, cap)) != GIF_R_OK) { return res; }
    decom->code_width = min_code_width;
    return GIF_R_OK;
}
