#include "gif/header.h"
#include "gif/lzw.h"
#include "gif/read.h"
#include "gif/log.h"
#include <gif/endian.h>

#include <stdio.h>
#include <assert.h>

FILE *gif_log_file;

int main(int argc, const char * const argv[]) {
    gif_log_file = stdout;
    
    
    gif_t gif;
    gif_err_t res;
    if((res = gif_open_file("../earth.gif", &gif)) != GIF_R_OK) {
        perror(gif_err_str(res));
        return res;
    }
    printf(
        "Header: %.6s\n"
        "Logical Width: %u\n"
        "Logical Height: %u\n"
        "Has GCT: %u\n"
        "GCT sz: %u\n"
        "GCT entries: %u\n"
        "Has LCT: %u\n"
        "LCT sz: %u\n"
        "LCT entries: %u\n"
        "Num images: %zu\n"
        "imgw: %u\n"
        "imgh: %u\n",
        gif.header.header,
        gif.header.lwidth,
        gif.header.lheight,
        gif.header.has_gct,
        gif.header.gct_sz,
        gif_header_gct_entries_count(&gif.header),
        gif.blocks.images[0].descriptor.has_lct,
        gif.blocks.images[0].descriptor.lct_sz,
        gif_image_descriptor_lct_entries_count(&gif.blocks.images[0].descriptor),
        gif.blocks.len,
        gif.blocks.images[0].descriptor.width,
        gif.blocks.images[0].descriptor.height
    );
    
    gif_image_block_t *img = &gif.blocks.images[0];
    for(size_t i = 0; i < gif.header.lheight; ++i) {
        for(size_t j = 0; j < gif.header.lwidth; ++j) {
            if(img->buf[i * gif.header.lwidth + j] == 1) {
                printf("1");
            } else {
                printf("0");
            }
        }
        puts("");
    }
    return 0;
}
