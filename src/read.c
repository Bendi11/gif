#include "gif/color.h"
#include "gif/endian.h"
#include "gif/header.h"
#include <gif/read.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

gif_err_t gif_image_descriptor_read(gif_image_descriptor_t *img, FILE *file);

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
    
    bool trailer = false;
    while(!trailer) {
        uint8_t introducer = GIF_INTRODUCER_TRAILER;
        if(fread(&introducer, 1, sizeof(introducer), file) != 1) { return GIF_R_FERROR; }
        switch(introducer) {
            case GIF_INTRODUCER_TRAILER: {
                trailer = true;
                continue;
            } break;

            case GIF_INTRODUCER_IMG: {
                gif_image_descriptor_t descriptor = {0};
                descriptor.separator = introducer;
                gif_err_t res;
                if((res = gif_image_descriptor_read(&descriptor, file)) != GIF_R_OK) {
                    return res;
                }
                
                size_t len = descriptor.width + descriptor.height;
                gif_image_index_t *pxdata = calloc(sizeof(gif_image_index_t), len);
                if(pxdata == NULL) { return GIF_R_ALLOC; }
                if((res = gif_read_subblocks(file, pxdata, len)) != GIF_R_OK) { return res; }
                
                 
            } break;
        }
    }  

    return GIF_R_OK; 
}

gif_err_t gif_image_descriptor_read(gif_image_descriptor_t *img, FILE *file) {
    //Read all bytes but the introducer
    if(fread(&img->x, sizeof(*img) - sizeof(img->separator), 1, file) != 1) { return GIF_R_FERROR; }
    if(!IS_LITTLE_ENDIAN) {
        img->x = le_to_host16(img->x);
        img->y = le_to_host16(img->y);
        img->width = le_to_host16(img->width);
        img->height = le_to_host16(img->height);
    }

    return GIF_R_OK;
}

gif_err_t gif_read_subblocks(FILE *file, void *data, size_t len) {
    size_t idx = 0;
    for(;;) {
        uint8_t nbytes;
        if(fread(&nbytes, sizeof(nbytes), 1, file) != 1) { return GIF_R_FERROR; }
        if(nbytes == 0) {
            return GIF_R_OK;
        }
        
        if(idx + nbytes >= len) { return GIF_R_ALLOC; }
        if(fread(data + idx, 1, nbytes, file) != nbytes) { return GIF_R_FERROR; }
        idx += nbytes;
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
