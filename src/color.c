#include "gif/header.h"
#include <gif/color.h>

uint16_t gif_header_gct_entries_count(const gif_header_t *const header) {
    return (1UL << (header->gct_sz + 1));
}

uint16_t gif_header_gct_size_bytes(const gif_header_t *const header) {
    return sizeof(gif_color_t) * gif_header_gct_entries_count(header);
}

uint16_t gif_image_descriptor_lct_entries_count(const gif_image_descriptor_t *const header) {
    return (1UL << (header->lct_sz + 1));
}

uint16_t gif_image_descriptor_lct_size_bytes(const gif_image_descriptor_t *const header) {
    return sizeof(gif_color_t) * gif_image_descriptor_lct_entries_count(header);
}

