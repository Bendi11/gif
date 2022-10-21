#include "gif/header.h"
#include "gif/read.h"
#include "gif/color.h"
#include <malloc.h>


gif_err_t gif_read_color_table(FILE *file, gif_color_table_t *tbl, uint16_t entries) {
    tbl->len = entries;
    tbl->entries = calloc(tbl->len, sizeof(*tbl->entries));
    if(tbl->entries == NULL) { return GIF_R_ALLOC; }
    if(fread(tbl->entries, sizeof(*tbl->entries), tbl->len, file) != tbl->len) {
        return GIF_R_FERROR;
    }
    return GIF_R_OK;
}


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

