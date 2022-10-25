#include "gif/lzw.h"
#include "gif/buf.h"
#include "gif/error.h"
#include "gif/log.h"
#include <stdint.h>
#include <string.h>

lzw_tbl_entry_t first_entry(lzw_decompressor_t *decom, uint16_t code) {
    while(decom->tbl.entries[code].prev != LZW_INVALID_TBL_IDX) {
        code = decom->tbl.entries[code].prev;
    }
    return decom->tbl.entries[code];
}

void output_entry(lzw_decompressor_t *decom, uint16_t code) {
    uint16_t len = decom->tbl.entries[code].len;
    while(code != LZW_INVALID_TBL_IDX) {
        decom->output[decom->tbl.entries[code].len - 1] = decom->tbl.entries[code].index;
        if(decom->tbl.entries[code].prev == code) {
            LOG("Circular reference for code %.02X", code);
        }
        code = decom->tbl.entries[code].prev;
    }
    decom->output += len;
}

gif_err_t lzw_decompressor_feed(lzw_decompressor_t *decom, void *bytes, uint32_t nbytes) {
    gif_err_t res;

    bitbuffer_t reader;
    bitbuffer_new(&reader, bytes);
    while(reader.bitpos < nbytes * 8 + decom->code_width) {
        uint16_t code = bitbuffer_readn(&reader, decom->code_width);

        if(code == decom->clear) {
            LOG("LZW code clear @%lX", reader.bitpos / 8);
            lzw_decompressor_start(decom, decom->original_code_width, decom->output); 
            continue;
        }
        if(code == decom->stop) {
            if(reader.buf - (uint8_t*)bytes != nbytes - 1) {
                LOG("LZW earlyexit %zu, nb = %u", reader.buf - (uint8_t*)bytes, nbytes);
                return GIF_R_FERROR;
            }
            break;
        }

        /*lzw_tbl_entry_t entry;
        if(code < decom->tbl.free_entry) {
            if(decom->prev == LZW_INVALID_TBL_IDX) {
                decom->prev = code;
                continue;
            }

            output_entry(decom, code);

            entry = (lzw_tbl_entry_t){
                .index = first_entry(decom, code).index,
                .prev = decom->prev,
                .len = decom->tbl.entries[decom->prev].len + 1
            };
            decom->tbl.entries[decom->tbl.free_entry] = entry;
            decom->tbl.free_entry += 1;

            decom->prev = code;
        } else {
            if(decom->prev == LZW_INVALID_TBL_IDX) {
                decom->prev = code;
                continue;
            }

            entry = (lzw_tbl_entry_t){
                .index = first_entry(decom, decom->prev).index,
                .prev = decom->prev,
                .len = decom->tbl.entries[decom->prev].len + 1
            };
            decom->prev = decom->tbl.free_entry;
            decom->tbl.entries[decom->tbl.free_entry] = entry;
            output_entry(decom, decom->tbl.free_entry);
            decom->tbl.free_entry += 1;
        }*/


        

        if(decom->prev < LZW_INVALID_TBL_IDX && decom->code_width < 12) {
            if(code > decom->tbl.free_entry) {
                /*lzw_tbl_entry_t first = first_entry(decom, decom->prev);
                lzw_tbl_entry_t *entry = &decom->tbl.entries[decom->tbl.free_entry];
                entry->prev = decom->prev;
                entry->index = first.index;
                entry->len = decom->tbl.entries[decom->prev].len + 1;
                decom->tbl.free_entry += 1;*/
                LOG("LZW code %.02X is greater than dictionary index %.02X", code, decom->tbl.free_entry);
                return GIF_R_ALLOC;
            } else {
                uint16_t ptr;
                if(code == decom->tbl.free_entry) {
                    ptr = decom->prev;
                } else {
                    ptr = code;
                }

                while(decom->tbl.entries[ptr].prev != LZW_INVALID_TBL_IDX) {
                    //LOG("looking for previous of %.04X", ptr);
                    ptr = decom->tbl.entries[ptr].prev;
                }

                lzw_tbl_entry_t *entry = &decom->tbl.entries[decom->tbl.free_entry];
                entry->index = decom->tbl.entries[ptr].index;
                entry->prev = decom->prev;
                entry->len = decom->tbl.entries[decom->prev].len + 1;
                decom->tbl.free_entry += 1;

                if(decom->tbl.free_entry == ((1UL << decom->code_width)) && decom->code_width < 11) {
                    decom->code_width += 1;
                    LOG("LZW increased code width to %u", decom->code_width);
                }
            }
        }

        
        
        LOG("LZW code %.04X", code);
        decom->prev = code;
        uint16_t len = decom->tbl.entries[code].len;
        while(code != LZW_INVALID_TBL_IDX) {
            decom->output[decom->tbl.entries[code].len - 1] = decom->tbl.entries[code].index;
            if(decom->tbl.entries[code].prev == code) {
                LOG("Circular reference for code %.02X", code);
            }
            code = decom->tbl.entries[code].prev;
        }
        decom->output += len;

    }
    return GIF_R_OK;
}

uint16_t bitbuffer_readn(bitbuffer_t *buf, uint8_t width) {
    uint16_t val = 0;
    
    for (uint16_t i = 0; i < width; i++) {
        uint8_t bit = (*buf->buf & buf->mask) ? 1 : 0;
        buf->mask <<= 1;

        if (buf->mask == 0x100)  {
            buf->mask = 0x01;
            buf->buf++;
        }

        val = val | ( bit << i );
    }

    buf->bitpos += width;

    return val;
}

void bitbuffer_new(bitbuffer_t *buf, void *bytes) {
    buf->buf = (uint8_t*)bytes;
    buf->bitpos = 0;
    buf->mask = 0x01;
}

void lzw_decompressor_new(lzw_decompressor_t *decom) {
    decom->output = NULL;
    decom->tbl.free_entry = 0;
    decom->prev = LZW_INVALID_TBL_IDX; 
    decom->code_width = 12;
}

void lzw_decompressor_start(lzw_decompressor_t *decom, uint8_t code_width, void *out) {
    decom->output = (uint8_t*)out;
    uint16_t first_code = (1UL << code_width);
    memset(decom->tbl.entries, 0, LZW_MAX_TBL_ENTRIES * sizeof(decom->tbl.entries[0])); 
    decom->clear = first_code;
    decom->stop = decom->clear + 1;
    decom->original_code_width = code_width;
    
    for(uint16_t i = 0; i < first_code; ++i) {
        decom->tbl.entries[i].index = (uint8_t)i;
        decom->tbl.entries[i].prev = LZW_INVALID_TBL_IDX;
        decom->tbl.entries[i].len = 1;
    }

    decom->tbl.free_entry = first_code + 2;

    decom->code_width = code_width + 1;
    decom->prev = LZW_INVALID_TBL_IDX;
}

void lzw_decompressor_finish(lzw_decompressor_t *decom) {
    decom->output = NULL;
    decom->prev = LZW_INVALID_TBL_IDX;
}
