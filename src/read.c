#include "gif/endian.h"
#include "gif/header.h"
#include <gif/read.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

gif_err_t gif_open_file(const char *const path, gif_t *gif) {
    FILE *file = fopen(path, "r");
    if(file == NULL) { return GIF_R_FERROR; }
    gif_err_t result = gif_read_file(file, gif);
    fclose(file);
    return result;
}

gif_err_t gif_read_file(FILE *file, gif_t *gif) {
    if(fread(&gif->header, sizeof(gif->header), 1, file) != 1) { return GIF_R_FERROR; }
    if(!IS_LITTLE_ENDIAN) {
        le_to_host(&gif->header.header, sizeof(gif->header.header));
        gif->header.lwidth = le_to_host16(gif->header.lwidth);
        gif->header.lheight = le_to_host16(gif->header.lheight);
    }
    
    if(memcmp(gif->header.header, "GIF89a", 6) != 0 && memcmp(gif->header.header, "GIF87a", 6) != 0) {
        return GIF_R_INVALID_HEADER;
    }

    if(gif->header.has_gct != 0) {
        gif->gct.len = gif_header_gct_entries_count(&gif->header);
        gif->gct.entries = calloc(gif->gct.len, sizeof(*gif->gct.entries));
        if(gif->gct.entries == NULL) { return GIF_R_ALLOC; }
        if(fread(gif->gct.entries, sizeof(*gif->gct.entries), gif->gct.len, file) != gif->gct.len) {
            return GIF_R_FERROR;
        }
    } else {
        gif->gct.entries = NULL;
        gif->gct.len = 0;
    }

    for(;;) {
        uint8_t introducer = GIF_INTRODUCER_TRAILER;
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

char const *const gif_err_str(gif_err_t err) {
    switch(err) {
        case GIF_R_OK: return "GIF file ok";
        case GIF_R_INVALID_HEADER: return "Invalid GIF header";
        case GIF_R_FERROR: return "Failed to read GIF file";
        case GIF_R_ALLOC: return "Failed to allocate memory";
        default: return "(invalid error code)";
    }
}
