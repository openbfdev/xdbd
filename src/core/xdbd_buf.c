#include <xdbd_string.h>
#include <xdbd_pool.h>
#include <xdbd.h>
#include <xdbd_buf.h>
#include <string.h>
xdbd_buf_t *xdbd_create_buf(xdbd_pool_t *pool, size_t size) {
    xdbd_buf_t *buf;

    buf = xdbd_palloc(pool, sizeof(xdbd_buf_t));
    if (buf == NULL) {
        return NULL;
    }

    buf->start = xdbd_palloc(pool, size);
    if (buf->start == NULL) {
        return NULL;
    }

    buf->end = buf->start + size;
    buf->pos = buf->start;
    buf->last = buf->start;
    return buf;
}

xdbd_buf_t *xdbd_buf_append_mem(xdbd_buf_t *b, xdbd_pool_t *pool, const void *mem, size_t len) {
    unsigned capacity, size;
    u_char *p;
    if (len > (size_t)(b->end - b->last)) {

		size = xdbd_buf_size(b);

		capacity = xdbd_buf_capacity(b);
		capacity <<= 1;

		if (capacity < (size + len)) {
			capacity = size + len;
		}

		p = xdbd_palloc(pool, capacity);
		if (p == NULL) {
			return NULL;
		}


		b->last = xdbd_cpymem(p, b->pos, size);;

		b->start = b->pos = p;
		b->end = p + capacity;
	}

	b->last = xdbd_cpymem(b->last, mem, len);

    return b;
}

xdbd_buf_t *xdbd_buf_append_cstring(xdbd_buf_t *b, xdbd_pool_t *pool, const char *str) {
    return xdbd_buf_append_mem(b, pool, str, strlen(str));
}

xdbd_buf_t *xdbd_buf_append_string(xdbd_buf_t *b, xdbd_pool_t *pool, xdbd_str_t *str) {
    return xdbd_buf_append_mem(b, pool, str->data, str->size);
}

xdbd_buf_t *xdbd_buf_append_buf(xdbd_buf_t *b, xdbd_pool_t *pool, xdbd_buf_t *buf) {
    return xdbd_buf_append_mem(b, pool, buf->pos, xdbd_buf_size(buf));
}
