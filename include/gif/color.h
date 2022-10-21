#pragma once

#include <stdint.h>


/** \brief A single RGB color, appearing in a local or global color table */
typedef struct gif_color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} gif_color_t;

typedef uint8_t gif_image_index_t;
