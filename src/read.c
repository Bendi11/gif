#include "gif/endian.h"
#include <gif/read.h>
#include <stdio.h>

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
    return GIF_R_OK; 
}

char const *const gif_err_str(gif_err_t err) {
    switch(err) {
        case GIF_R_OK: return "GIF file ok";
        case GIF_R_INVALID_HEADER: return "Invalid GIF header";
        case GIF_R_FERROR: return "Failed to read GIF file";
        default: return "(invalid error code)";
    }
}
