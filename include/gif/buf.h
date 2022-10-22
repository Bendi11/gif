#pragma once


#include "gif/error.h"
#include <stdint.h>



/** A growable array of bytes */
typedef struct bytebuf_t {
    uint8_t *bytes;
    uint32_t cap;
    uint32_t len;
} bytebuf_t;

/** Create a new byte buffer with a default capacity */
gif_err_t bytebuf_new(bytebuf_t *buf);

/** Create a new byte buffer with the given capacity */
gif_err_t bytebuf_new_with_cap(bytebuf_t *buf, uint32_t cap);
