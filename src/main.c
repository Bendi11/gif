#include "gif/read.h"
#include <gif/endian.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, const char * const argv[]) {
    gif_t gif;
    gif_err_t res;
    if((res = gif_open_file("../test.gif", &gif)) != GIF_R_OK) {
        perror(gif_err_str(res));
        //return res;
    }
    printf(
        "Header: %.6s\n"
        "Logical Width: %u\n"
        "Logical Height: %u\n"
        "Has GCT: %u\n"
        "Image size: %zu\n",
        gif.header.header,
        gif.header.lwidth,
        gif.header.lheight,
        gif.header.has_gct,
        gif.blocks.images[0].buf_sz
    );
    return 0;
}
