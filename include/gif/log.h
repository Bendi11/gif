#pragma once

#define DEBUG (1)

#ifdef DEBUG

#include <stdio.h>

extern FILE *gif_log_file;

#define LOG(fmt, ...) do { fprintf(gif_log_file, "[%s:%d:%s()] " fmt "\n",  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); } while(0)

#else

#define LOG(...)

#endif
