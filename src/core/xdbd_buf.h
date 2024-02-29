#ifndef __XDBD_BUF__H__
#define __XDBD_BUF__H__
#include <xdbd.h>
#include <xdbd_string.h>
struct xdbd_buf_s {
    u_char *start;
    u_char *end;
    u_char *pos;
    u_char *last;
};

xdbd_buf_t *xdbd_create_buf(xdbd_pool_t *pool, size_t size);
xdbd_buf_t* xdbd_buf_append_mem(xdbd_buf_t *b, xdbd_pool_t *pool, const void *mem, size_t len);
xdbd_buf_t *xdbd_buf_append_string(xdbd_buf_t *b, xdbd_pool_t *pool, xdbd_str_t *str);
xdbd_buf_t *xdbd_buf_append_buf(xdbd_buf_t *b, xdbd_pool_t *pool, xdbd_buf_t *buf);
xdbd_buf_t *xdbd_buf_append_cstring(xdbd_buf_t *b, xdbd_pool_t *pool, const char *str);

#define xdbd_buf_size(b) (off_t) ((b)->last - (b)->pos)
#define xdbd_buf_capacity(b) (b)->end - (b)->start
#define xdbd_reset_buf(b) (b)->pos = (b)->last = (b)->start


#endif  /*__XDBD_BUF__H__*/
