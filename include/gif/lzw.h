#pragma once

#include "gif/buf.h"
#include <stdint.h>


#define LZW_FIRST_CODE (1UL << 8)
#define LZW_MAX_CODE_BITS 13

/** Maximum entries that can be indexed by a 12-bit code */
#define LZW_MAX_TBL_ENTRIES (1UL << LZW_MAX_CODE_BITS)

/** Abstraction over an array of bytes with a cursor tracking the bit position of the buffer, with methods to read and write to the buffer */
typedef struct bitbuffer_t {
    uint8_t *buf;
    uint32_t idx;
    uint8_t bitpos;
} bitbuffer_t;

/** An entry in an `lzw_decompressor_tbl_t` */
typedef struct lzw_tbl_entry_t {
    /** \brief Offset into the table's string slab that the first string appears */
    uint32_t offset;
    /** \brief Length of the string past the `offset` character that the string occupies */
    uint32_t len;
} lzw_tbl_entry_t;

/** A map of codes to their strings for an `lzw_decompressor_t` */
typedef struct lzw_decompressor_tbl_t {
    /** All entries in this table, the index into the table is the code */
    lzw_tbl_entry_t entries[LZW_MAX_TBL_ENTRIES];
    /** Using a bytebuf_t as a slab allocator for table strings */
    bytebuf_t strbuf;
    /** Index of the newest free entry */
    uint16_t free_entry;
} lzw_decompressor_tbl_t;

/**
 * Structure containing all state needed to decompress LZW encoded data with variable width codes
 */
typedef struct lzw_decompressor_t {
    /** Dictionary of all known LZW codes to their strings */
    lzw_decompressor_tbl_t tbl;
    /** Buffer to write output data to */
    bytebuf_t output;
    /** Current width to read input codes at */
    uint8_t code_width;
    /** The last-inserted string */
    uint16_t prev;
} lzw_decompressor_t;

/** \brief Consume `nbytes` from `bytes` and write the decompressed data to the output */
gif_err_t lzw_decompressor_feed(lzw_decompressor_t *decom, void *bytes, uint32_t nbytes);

/** \brief Create a new lzw_compressor_t with a default output capacity */
gif_err_t lzw_decompressor_new(lzw_decompressor_t *decom, uint8_t min_code_width);

/** \brief Create a new lzw_decompressor_t with the given output capacity */
gif_err_t lzw_decompressor_new_with_cap(lzw_decompressor_t *decom, uint32_t cap, uint8_t min_code_width);

/** \brief Read a value made of the given number of bits from this bit buffer */
uint16_t bitbuffer_readn(bitbuffer_t *buf, uint8_t width);

/** \brief Create a new bit buffer, backed by the given pointer (not copied) */
void bitbuffer_new(bitbuffer_t *buf, void *bytes);
