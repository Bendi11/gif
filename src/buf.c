#include "gif/buf.h"
#include "gif/error.h"
#include <malloc.h>
#include <string.h>

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

gif_err_t bytebuf_append(bytebuf_t *buf, const void *const bytes, uint32_t len) {
    gif_err_t res;
    if((res = bytebuf_grow(buf, len)) != GIF_R_OK) { return res; }
    memcpy(
        &buf->bytes[buf->len],
        bytes,
        len
    );

    buf->len += len;
    return GIF_R_OK;
}

gif_err_t bytebuf_grow(bytebuf_t *buf, uint32_t n) {
    if(buf->cap - buf->len <= n) {
        buf->bytes = reallocarray(buf->bytes, sizeof(*buf->bytes), buf->cap + n + 1); 
        if(buf->bytes == NULL) { return GIF_R_ALLOC; }
        buf->cap += n + 1;
    }

    return GIF_R_OK;
}

void bytebuf_free(bytebuf_t buf) {
    free(buf.bytes);
}
