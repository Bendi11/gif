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

gif_err_t gif_skip_subblocks(FILE *file);

gif_err_t gif_open_file(const char *const path, gif_t *gif) {
    FILE *file = fopen(path, "r");
    if(file == NULL) { return GIF_R_FERROR; }
    gif_err_t result = gif_read_file(file, gif);
    fclose(file);
    return result;
}

gif_err_t gif_read_color_table(FILE *file, gif_color_table_t *tbl, uint16_t entries) {
    tbl->len = entries;
    tbl->entries = calloc(tbl->len, sizeof(*tbl->entries));
    if(tbl->entries == NULL) { return GIF_R_ALLOC; }
    if(fread(tbl->entries, sizeof(*tbl->entries), tbl->len, file) != tbl->len) {
        return GIF_R_FERROR;
    }
    return GIF_R_OK;
}

gif_err_t gif_read_file(FILE *file, gif_t *gif) {
    gif_err_t res;
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
        uint16_t entries = gif_header_gct_entries_count(&gif->header);
        if((res = gif_read_color_table(file, &gif->gct, entries)) != GIF_R_OK) { return res; }
    } else {
        gif->gct.entries = NULL;
        gif->gct.len = 0;
    }

    if((res = gif_image_blocks_new(&gif->blocks)) != GIF_R_OK) { return res; }
    
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
                gif_image_block_t img = {0};
                img.descriptor.separator = introducer;
                if((res = gif_image_descriptor_read(&img.descriptor, file)) != GIF_R_OK) {
                    return res;
                }
                
                size_t len = img.descriptor.width + img.descriptor.height;
                
                if(img.descriptor.has_lct != 0) {
                    uint16_t entries = gif_image_descriptor_lct_entries_count(&img.descriptor);
                    if((res = gif_read_color_table(file, &img.lct, entries))) { return GIF_R_OK; }
                } else {
                    img.lct.entries = NULL;
                    img.lct.len = 0;
                }

                gif_image_index_t *pxdata = calloc(sizeof(gif_image_index_t), len);
                if(pxdata == NULL) { return GIF_R_ALLOC; }
                if((res = gif_read_subblocks(file, pxdata, len)) != GIF_R_OK) { return res; }
                
                img.buf = pxdata;
                img.buf_sz = len;
                gif_image_blocks_add(&gif->blocks, img);
            } break;

            case GIF_INTRODUCER_EXT: {
                uint8_t bytes[2] = {0};
                if(fread(bytes, sizeof(uint8_t), 2, file) != 2) { return GIF_R_FERROR; }
                fseek(file, bytes[1] + 1, SEEK_CUR);
                if((res = gif_skip_subblocks(file)) != GIF_R_OK) { return res; }
            }
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

gif_err_t gif_skip_subblocks(FILE *file) {
    for(;;) {
        uint8_t nbytes;
        if(fread(&nbytes, sizeof(nbytes), 1, file) != 1) { return GIF_R_FERROR; }
        if(nbytes == 0) { return GIF_R_OK; }
        fseek(file, nbytes, SEEK_CUR);
    }
}

gif_err_t gif_image_blocks_new(gif_image_blocks_t *blocks) {
    blocks->cap = 2;
    blocks->images = calloc(sizeof(*blocks->images), blocks->cap);
    if(blocks->images == NULL) { return GIF_R_ALLOC; }
    blocks->len = 0;
    return GIF_R_OK;
}

gif_err_t gif_image_blocks_add(gif_image_blocks_t *blocks, gif_image_block_t img) {
    if(blocks->len == blocks->cap) {
        blocks->cap <<= 1;
        blocks->images = reallocarray(blocks->images, sizeof(img), blocks->cap);
        if(blocks->images == NULL) { return GIF_R_ALLOC; }
    }
    blocks->images[blocks->len] = img;
    blocks->len += 1;
    return GIF_R_OK;
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
