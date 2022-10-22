#include "gif/buf.h"
#include "gif/error.h"
#include <malloc.h>

gif_err_t bytebuf_new(bytebuf_t *buf) {
    return bytebuf_new_with_cap(buf, 1024);
}

gif_err_t bytebuf_new_with_cap(bytebuf_t *buf, uint32_t cap) {
    buf->bytes = calloc(cap, sizeof(*buf->bytes));
    if(buf->bytes == NULL) { return GIF_R_ALLOC; }
    buf->cap = cap;
    buf->len = 0;
    return GIF_R_OK;
}
