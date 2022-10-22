#include "gif/lzw.h"
#include "gif/buf.h"
#include "gif/error.h"
#include "gif/log.h"
#include <string.h>

#define INVALID_CODE ((uint16_t)-1)


gif_err_t lzw_decompressor_feed(lzw_decompressor_t *decom, void *bytes, uint32_t nbytes) {
    gif_err_t res;

    bitbuffer_t reader;
    bitbuffer_new(&reader, bytes);
    while(reader.idx < nbytes - 1 || reader.bitpos >= decom->code_width) {
        uint16_t code = bitbuffer_readn(&reader, decom->code_width);
        lzw_tbl_entry_t *entry = &decom->tbl.entries[code];
        if(entry->len != 0) {
            uint8_t *str = &decom->tbl.strbuf.bytes[entry->offset];
            if((res = bytebuf_append(&decom->output, str, entry->len)) != GIF_R_OK) { return res; }
            decom->prev = entry->offset;
            
            if(decom->prev != INVALID_CODE) {
                lzw_tbl_entry_t *prev = &decom->tbl.entries[decom->prev];
                uint32_t new_offset = decom->tbl.strbuf.len;
                uint32_t new_len = prev->len + 1;

                if((res = bytebuf_append(&decom->tbl.strbuf, &decom->tbl.strbuf.bytes[prev->offset], prev->len)) != GIF_R_OK) { return res; }
                if((res = bytebuf_append(&decom->tbl.strbuf, str, 1)) != GIF_R_OK) { return res; }
                decom->tbl.entries[decom->tbl.free_entry].offset = new_offset;
                decom->tbl.entries[decom->tbl.free_entry].len = new_len;
                decom->tbl.free_entry += 1;
            }
        } else {
            if(decom->prev == INVALID_CODE) { LOG("GOT UNKNOWN CODE %X BUT PREVIOUS HAS NOT BEEN SET", code); }
            lzw_tbl_entry_t *prev = &decom->tbl.entries[decom->prev];
            uint32_t new_offset = decom->tbl.strbuf.len;
            uint32_t new_len = prev->len + 1;
            if((res = bytebuf_append(&decom->tbl.strbuf, &decom->tbl.strbuf.bytes[prev->offset], prev->len)) != GIF_R_OK) { return res; }
            if((res = bytebuf_append(&decom->tbl.strbuf, &decom->tbl.strbuf.bytes[prev->offset], 1)) != GIF_R_OK) { return res; }
            decom->tbl.entries[decom->tbl.free_entry].offset = new_offset;
            decom->tbl.entries[decom->tbl.free_entry].len = new_len;
            decom->tbl.free_entry += 1;

            if((res = bytebuf_append(&decom->output, &decom->tbl.strbuf.bytes[new_offset], new_len)) != GIF_R_OK) { return res; }
            decom->prev = new_offset;
        }
    }

    return GIF_R_OK;
}

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
    if((res = bytebuf_new_with_cap(&decom->tbl.strbuf, 512)) != GIF_R_OK) { return res; }
    memset(&decom->tbl.entries[LZW_FIRST_CODE], 0, (LZW_MAX_TBL_ENTRIES - LZW_FIRST_CODE) * sizeof(decom->tbl.entries[0]));
    uint8_t i = 0;
    do {
        decom->tbl.strbuf.bytes[i] = i;
        decom->tbl.entries[i].offset = i;
        decom->tbl.entries[i].len = 1;
        decom->tbl.strbuf.len += 1;
        i += 1;
    } while(i != 0);
    decom->tbl.free_entry = LZW_FIRST_CODE;
    decom->code_width = min_code_width;
    decom->prev = INVALID_CODE; 
    return GIF_R_OK;
}
