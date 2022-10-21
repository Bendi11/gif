#pragma once

#include <stdint.h>

/**
 * \brief GIF file header that must be present in all files
 */
typedef struct gif_header_t {
    /** \brief Must be GIF87a or GIF89a */
    uint8_t header[6];
    uint16_t lwidth;
    uint16_t lheight;
    uint8_t has_gct : 1;
    uint8_t gct_color_resolution : 2;
    uint8_t gct_sort : 1;
    uint8_t gct_sz : 2;
    uint8_t bg_color;
    uint8_t default_aspect_ratio;
} gif_header_t;
