#ifndef __CONNECTION__H__
#define __CONNECTION__H__

#include <xdbd.h>

struct xdbd_connection_s {
    int fd;
    xdbd_event_t *read;
    xdbd_event_t *write;
};

int xdbd_connection_init(xdbd_t *xdbd);

#endif  /*__CONNECTION__H__*/
