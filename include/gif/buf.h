#pragma once


#include "gif/error.h"
#include <stdint.h>



/** A growable array of bytes */
typedef struct bytebuf_t {
    uint8_t *bytes;
    uint32_t cap;
    uint32_t len;
} bytebuf_t;

/** Append `len` bytes from `bytes` onto the end of this `bytebuf_t` */
gif_err_t bytebuf_append(bytebuf_t *buf, void const * const bytes, uint32_t len);

/** Ensure that the given bytebuf_t can hold `n` more elements past the element at `len` */
gif_err_t bytebuf_grow(bytebuf_t *buf, uint32_t n);

/** Create a new byte buffer with a default capacity */
gif_err_t bytebuf_new(bytebuf_t *buf);

/** Create a new byte buffer with the given capacity */
gif_err_t bytebuf_new_with_cap(bytebuf_t *buf, uint32_t cap);

/** Free memory allocated for the given `bytebuf_t` */
void bytebuf_free(bytebuf_t buf);
