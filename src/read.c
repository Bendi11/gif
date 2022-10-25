#include "gif/buf.h"
#include "gif/color.h"
#include "gif/endian.h"
#include "gif/error.h"
#include "gif/header.h"
#include "gif/log.h"
#include "gif/lzw.h"
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
    lzw_decompressor_t dec;
    lzw_decompressor_new(&dec);

    while(!trailer) {
        uint8_t introducer = GIF_INTRODUCER_TRAILER;
        if(fread(&introducer, 1, sizeof(introducer), file) != 1) { return GIF_R_FERROR; }
        switch(introducer) {
            case GIF_INTRODUCER_TRAILER: {
                LOG("trailer @ %lX", ftell(file) - 1);
                trailer = true;
                continue;
            } break;

            case GIF_INTRODUCER_IMG: {
                LOG("img @ %lX", ftell(file) - 1);
                gif_image_block_t img = {0};
                img.descriptor.separator = introducer;
                if((res = gif_image_descriptor_read(&img.descriptor, file)) != GIF_R_OK) {
                    return res;
                }
                
                size_t len = img.descriptor.width * img.descriptor.height;
                
                if(img.descriptor.has_lct != 0) {
                    uint16_t entries = gif_image_descriptor_lct_entries_count(&img.descriptor);
                    if((res = gif_read_color_table(file, &img.lct, entries))) { return GIF_R_OK; }
                } else {
                    img.lct.entries = NULL;
                    img.lct.len = 0;
                }

                uint8_t *pxdata = malloc(len);

                uint8_t min_lzw_code;
                LOG("img 2 read min lzw code width %s", "");
                if(fread(&min_lzw_code, sizeof(min_lzw_code), 1, file) != 1) { return GIF_R_FERROR; }
                LOG("Beginning decompression with min lzw code width %u", min_lzw_code);
                lzw_decompressor_start(&dec, min_lzw_code, pxdata);
                //bytebuf_t buf;
                //bytebuf_new(&buf);


                //if((res = gif_read_subblocks_to(file, &buf)) != GIF_R_OK) { return res; }
                

                //if((res = lzw_decompressor_feed(&dec, buf.bytes, buf.len)) != GIF_R_OK) { return res; }

                if((res = gif_decompress_subblocks(file, &dec)) != GIF_R_OK) { return res; }
                lzw_decompressor_finish(&dec);
                
                img.buf = pxdata;
                img.buf_sz = len;
                if((res = gif_image_blocks_add(&gif->blocks, img)) != GIF_R_OK) { return res; }
            } break;

            case GIF_INTRODUCER_EXT: {
                LOG("ext @ %lX", ftell(file) - 1);
                uint8_t bytes[2] = {0};
                if(fread(bytes, sizeof(uint8_t), 2, file) != 2) { return GIF_R_FERROR; }
                LOG("ext lbl = %X, bs = %X @ %lX", bytes[0], bytes[1], ftell(file) - 1);
                fseek(file, bytes[1], SEEK_CUR);
                if((res = gif_skip_subblocks(file)) != GIF_R_OK) { return res; }
            } break;

            default: {
                LOG("Unknown introducer %X @ %lX", introducer, ftell(file) - 1);
            } break;
        }
    }

    return GIF_R_OK; 
}

gif_err_t gif_image_descriptor_read(gif_image_descriptor_t *img, FILE *file) {
    //Read all bytes but the introducer
    if(fread(img + sizeof(img->separator), sizeof(*img) - sizeof(img->separator), 1, file) != 1) { return GIF_R_FERROR; }
    if(!IS_LITTLE_ENDIAN) {
        img->x = le_to_host16(img->x);
        img->y = le_to_host16(img->y);
        img->width = le_to_host16(img->width);
        img->height = le_to_host16(img->height);
    }

    return GIF_R_OK;
}

gif_err_t gif_decompress_subblocks(FILE *file, lzw_decompressor_t *dec) {
    gif_err_t res; 
    uint8_t block_buf[256] = {0};
    for(;;) {
        uint8_t nbytes;
        if(fread(&nbytes, sizeof(nbytes), 1, file) != 1) { return GIF_R_FERROR; }
        LOG("Decompressing subblock of sz %u @ %lX", nbytes, ftell(file) - 1);
        if(nbytes == 0) { return GIF_R_OK; }

        if(fread(block_buf, 1, nbytes, file) != nbytes) { return GIF_R_FERROR; }
        if((res = lzw_decompressor_feed(dec, block_buf, nbytes)) != GIF_R_OK) { return res; }
    }
}

gif_err_t gif_read_subblocks_to(FILE *file, bytebuf_t *buf) {
    gif_err_t res;
    uint8_t tmp[256] = {0};
    for(;;) {
        uint8_t nbytes;
        if(fread(&nbytes, sizeof(nbytes), 1, file) != 1) { return GIF_R_FERROR; }
        LOG("Subblock of sz %u @ %lX", nbytes, ftell(file) - 1);
        if(nbytes == 0) {
            return GIF_R_OK;
        }
        

        if(fread(tmp, 1, nbytes, file) != nbytes) { return GIF_R_FERROR; }
        if((res = bytebuf_append(buf, tmp, nbytes)) != GIF_R_OK) { return res; }
    }

    return GIF_R_OK;

}

gif_err_t gif_read_subblocks(FILE *file, void *data, size_t len) {
    size_t idx = 0;
    for(;;) {
        uint8_t nbytes;
        if(fread(&nbytes, sizeof(nbytes), 1, file) != 1) { return GIF_R_FERROR; }
        LOG("Subblock of sz %u @ %lX", nbytes, ftell(file) - 1);
        if(nbytes == 0) {
            return GIF_R_OK;
        }
        
        if(idx + nbytes >= len) { return GIF_R_UNEXPECTED_SUBBLOCKS; }
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
        case GIF_R_UNEXPECTED_SUBBLOCKS: return "Unexpected subblocks";
        default: return "(invalid error code)";
    }
}
