#pragma once

#include "gif/color.h"
#include "gif/header.h"
#include <stdint.h>
#include <stdio.h>


enum {
    GIF_R_OK,
    GIF_R_FERROR,
    GIF_R_INVALID_HEADER,
    GIF_R_ALLOC,
};


typedef uint8_t gif_err_t;

/** \brief Get a NULL-terminated string representing an error message for the given error code */
char const *const gif_err_str(gif_err_t err);

/** \brief A GIF local or global color table mapping color indexes per pixel to RGB colors */
typedef struct gif_color_table_t {
    gif_color_t *entries;
    uint16_t len;
} gif_color_table_t;

/** \brief Structure containing an image descriptor and a buffer containing all de-interleaved color indices for each pixel */
typedef struct gif_image_block_t {
    gif_image_descriptor_t descriptor;
    gif_image_index_t *buf;
    size_t buf_sz;
} gif_image_block_t;

/** \brief Resizable buffer containing multiple image blocks */
typedef struct gif_image_blocks_t {
    gif_image_block_t *images;
    size_t len;
    size_t cap;
} gif_image_blocks_t;

/**
 * Structure representing a parsed GIF file that can be read from or written to a file
 */
typedef struct gif_t {
    /** Header followed by a Logical Screen Descriptor */
    gif_header_t header;
    /** Global color table, has length 0 if there is no GCT */
    gif_color_table_t gct;
    /** Buffer containing all images in the GIF file */
    gif_image_blocks_t blocks;
} gif_t;

/** \brief Create a new `gif_image_blocks_t` structure with a default capacity */
gif_err_t gif_image_blocks_new(gif_image_blocks_t *blocks);

/** \brief **Move** the given image structure into this buffer */
gif_err_t gif_image_blocks_add(gif_image_blocks_t *blocks, gif_image_block_t img);

/** \brief Open the gif file at the given path and attempt to parse a GIF file  */
gif_err_t gif_open_file(const char *const path, gif_t *gif);

/** \brief Use an already open file to attempt to parse a GIF file */
gif_err_t gif_read_file(FILE *file, gif_t *gif);

/** \brief Read a linked list of data subblocks into `data`, ensuring that read bytes do not exceed `len` */
gif_err_t gif_read_subblocks(FILE *file, void *data, size_t len);
