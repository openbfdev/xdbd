#ifndef __CONNECTION__H__
#define __CONNECTION__H__

#include "xdbd_pool.h"
#include <xdbd.h>
#include <sys/socket.h>

typedef void (*xdbd_connection_handler_pt)(xdbd_connection_t *c);

struct xdbd_listening_s {
    xdbd_socket_t fd;
    unsigned type;
    struct sockaddr    *sockaddr;
    socklen_t           socklen;    /* size of sockaddr */
    xdbd_connection_t *connection;

    xdbd_connection_handler_pt handler;

    unsigned backlog;
    unsigned listen:1;

    //connection_t and event_t have to memzero .only this
    xdbd_t *xdbd;
};

struct xdbd_connection_s {
    xdbd_pool_t *pool;
    xdbd_socket_t fd;
    xdbd_event_t *read;
    xdbd_event_t *write;

    xdbd_recv_pt         recv;
    xdbd_send_pt         send;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;

    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;


    void *data;
    xdbd_listening_t *listening;

    off_t sent;

    unsigned destroyed:1;
};

#define ADB_CONNECTION_DEFAULT_PORT 5555

xdbd_connection_t *xdbd_get_connection(xdbd_t *xdbd, xdbd_socket_t s);
void xdbd_free_connection(xdbd_t *xdbd, xdbd_connection_t *c);
void xdbd_close_connection(xdbd_t *xdbd, xdbd_connection_t *c);

xdbd_listening_t *xdbd_create_listening(xdbd_t *xdbd, struct sockaddr *sockaddr, socklen_t socklen);
int xdbd_open_listening_sockets(xdbd_t *xdbd);


ssize_t xdbd_unix_recv(xdbd_connection_t *c, u_char *buf, size_t size);
ssize_t xdbd_unix_send(xdbd_connection_t *c, u_char *buf, size_t size);
#endif  /*__CONNECTION__H__*/
