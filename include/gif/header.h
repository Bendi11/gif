#pragma once

#include "gif/color.h"
#include <stdint.h>

enum {
    GIF_INTRODUCER_IMG = 0x2C,
    GIF_INTRODUCER_EXT = 0x21,
    GIF_INTRODUCER_TRAILER = 0x3B
};

#pragma pack(push,1)

/**
 * \brief GIF file header that must be present in all files
 */
typedef struct gif_header_t {
    /** \brief Must be GIF87a or GIF89a */
    uint8_t header[6];
    uint16_t lwidth;
    uint16_t lheight;
    //Packed into one byte 
    union {
        struct {
            uint8_t gct_sz : 3;
            uint8_t gct_sort : 1;
            uint8_t gct_color_resolution : 3;
            uint8_t has_gct : 1;
        };
        uint8_t packed;
    };

    gif_color_index_t bg_color;
    uint8_t default_aspect_ratio;
} gif_header_t;


/** Header of an image descriptor header */
typedef struct gif_image_descriptor_t {
    //Must be 0x2C
    uint8_t separator;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    //Packed into one byte
    union {
        struct {
            uint8_t has_lct : 1;
            uint8_t interlace : 1;
            uint8_t lct_sorted : 1;
            uint8_t _reserved : 2;
            uint8_t lct_sz : 3;
        };
        uint8_t packed;
    };
} gif_image_descriptor_t;


#pragma pack(pop)

/** \brief Get the number of Global Color Table entries in the given GIF header */
uint16_t gif_header_gct_entries_count(gif_header_t const *const header);

/** \brief Get the size of the Global Color Table in bytes for the given GIF header */
uint16_t gif_header_gct_size_bytes(gif_header_t const *const header);

/** \brief Get the number of Locla Color Table entries in the given image descriptor */
uint16_t gif_image_descriptor_lct_entries_count(gif_image_descriptor_t const *const header);

/** \brief Get the size of the Local Color Table in bytes for the given image descriptor */
uint16_t gif_image_descriptor_lct_size_bytes(gif_image_descriptor_t const *const header);
