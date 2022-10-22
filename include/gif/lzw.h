#pragma once

#include <stdint.h>

/** Abstraction over an array of bytes with a cursor tracking the bit position of the buffer, with methods to read and write to the buffer */
typedef struct bitbuffer_t {
    uint8_t *buf;
    uint32_t idx;
    uint8_t bitpos;
} bitbuffer_t;

/**
 * Structure containing all state needed to decompress LZW encoded data with variable width codes
 */
typedef struct lzw_decompressor_t {
    /** Bit reader to get codes from */
    bitbuffer_t buf;
    /** Current width to read input codes at */
    uint8_t code_width; 
} lzw_decompressor_t;

/** \brief Read a value made of the given number of bits from this bit buffer */
uint16_t bitbuffer_readn(bitbuffer_t *buf, uint8_t width);

/** \brief Create a new bit buffer, backed by the given pointer (not copied) */
void bitbuffer_new(bitbuffer_t *buf, void *bytes);
