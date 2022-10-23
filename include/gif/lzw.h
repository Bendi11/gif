#pragma once

#include "gif/buf.h"
#include <stdint.h>


#define LZW_MAX_CODE_BITS 12

/** Maximum entries that can be indexed by a 12-bit code */
#define LZW_MAX_TBL_ENTRIES (1UL << LZW_MAX_CODE_BITS)

#define LZW_INVALID_TBL_IDX (4096)

/** Abstraction over an array of bytes with a cursor tracking the bit position of the buffer, with methods to read and write to the buffer */
typedef struct bitbuffer_t {
    uint8_t *buf;
    uint64_t bitpos;
    uint16_t mask;
    uint16_t bit;
} bitbuffer_t;

/** An entry in an `lzw_decompressor_tbl_t` */
typedef struct lzw_tbl_entry_t {
    /** The index to output for this entry */
    uint8_t index;
    /** \brief Entry that precedes this one */
    uint16_t prev;
    /** \brief Length of the string */
    uint16_t len;
} lzw_tbl_entry_t;

/** A map of codes to their strings for an `lzw_decompressor_t` */
typedef struct lzw_decompressor_tbl_t {
    /** All entries in this table, the index into the table is the code */
    lzw_tbl_entry_t entries[LZW_MAX_TBL_ENTRIES];
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
    uint8_t *output;
    /** Current width to read input codes at */
    uint8_t code_width;
    /** Code width to reset to */
    uint8_t original_code_width;
    /** The last-inserted string */
    uint16_t prev;
    /** Special code */
    uint16_t clear;
    /** Special code */
    uint16_t stop;
} lzw_decompressor_t;

/** \brief Consume `nbytes` from `bytes` and write the decompressed data to the output */
gif_err_t lzw_decompressor_feed(lzw_decompressor_t *decom, void *bytes, uint32_t nbytes);

/** \brief Create a new lzw_compressor_t with a default output capacity */
void lzw_decompressor_new(lzw_decompressor_t *decom);

/** \brief Start decompression with the given starting code width */
void lzw_decompressor_start(lzw_decompressor_t *decom, uint8_t code_width, void *output);

/** \brief Reset the given LZW decompressor dictionary and return an owned pointer to the decompressed data */
void lzw_decompressor_finish(lzw_decompressor_t *decom);

/** \brief Read a value made of the given number of bits from this bit buffer */
uint16_t bitbuffer_readn(bitbuffer_t *buf, uint8_t width);

/** \brief Create a new bit buffer, backed by the given pointer (not copied) */
void bitbuffer_new(bitbuffer_t *buf, void *bytes);
