#pragma once

#include <stdint.h>


enum {
    GIF_R_OK,
    GIF_R_FERROR,
    GIF_R_INVALID_HEADER,
    GIF_R_ALLOC,
};


typedef uint8_t gif_err_t;

/** \brief Get a NULL-terminated string representing an error message for the given error code */
char const *const gif_err_str(gif_err_t err);


