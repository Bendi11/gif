#pragma once

#include <stdint.h>
#include "gif/error.h"
#include <stdio.h>

/** \brief A single RGB color, appearing in a local or global color table */
typedef struct gif_color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} gif_color_t;


/** \brief A GIF local or global color table mapping color indexes per pixel to RGB colors */
typedef struct gif_color_table_t {
    gif_color_t *entries;
    uint16_t len;
} gif_color_table_t;


/** \brief Read a color table with the specified number of entries from the given file */
gif_err_t gif_read_color_table(FILE *file, gif_color_table_t *tbl, uint16_t entries);

typedef uint8_t gif_color_index_t;
