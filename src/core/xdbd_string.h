#ifndef __XDBD_STRING__H__
#define __XDBD_STRING__H__
#include <xdbd.h>

typedef struct {
    ssize_t size;
    u_char *data;
} xdbd_str_t;

#define xdbd_string(str)     { sizeof(str) - 1, (u_char *) str }
#define xdbd_null_string     { 0, NULL }

#endif  /*__XDBD_STRING__H__*/
