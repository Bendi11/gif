#pragma once

#include "gif/header.h"
#include <stdint.h>
#include <stdio.h>


enum {
    GIF_R_OK,
    GIF_R_FERROR,
    GIF_R_INVALID_HEADER,  
};


typedef uint8_t gif_err_t;


char const *const gif_err_str(gif_err_t err);

/**
 * Structure representing a parsed GIF file that can be read from or written to a file
 */
typedef struct gif_t {
    gif_header_t header;
} gif_t;

/** \brief Open the gif file at the given path and attempt to parse a GIF file  */
gif_err_t gif_open_file(const char *const path, gif_t *gif);

/** \brief Use an already open file to attempt to parse a GIF file */
gif_err_t gif_read_file(FILE *file, gif_t *gif);
