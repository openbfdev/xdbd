#ifndef __ADBD__H__
#define __ADBD__H__


#include <bfdev/array.h>
#include "xdbd_pool.h"
#include <bfdev/allocpool.h>
#include <xdbd_config.h>
#include <netinet/in.h>
#define XDBD_OK 0
#define XDBD_ERR -1

#define xdbd_msec_t unsigned

/*socket*/
#define xdbd_socket          socket
#define xdbd_close_socket    close

typedef int xdbd_socket_t;

typedef union {
    struct sockaddr           sockaddr;
    struct sockaddr_in        sockaddr_in;
#if (XDBD_HAVE_INET6)
    struct sockaddr_in6       sockaddr_in6;
#endif
#if (XDBD_HAVE_UNIX_DOMAIN)
    struct sockaddr_un        sockaddr_un;
#endif
} xdbd_sockaddr_t;

typedef struct adb_command_s adb_command_t;
typedef struct adb_packet_s  adb_packet_t;
typedef struct xdbd_connection_s  xdbd_connection_t;
typedef struct xdbd_listening_s  xdbd_listening_t;
typedef struct xdbd_event_s xdbd_event_t;
typedef struct adb_pcmd_s adb_pcmd_t;

typedef struct xdbd_s xdbd_t;

struct xdbd_s {
    unsigned connection_n;
    xdbd_connection_t *connections;
    unsigned free_connection_n;
    xdbd_connection_t *free_connections;

    xdbd_event_t *write_events;
    xdbd_event_t *read_events;

    bfdev_array_t listening;
    xdbd_pool_t *pool;
};

#define xdbd_memzero(buf, n)       (void) memset(buf, 0, n)
#define xdbd_memcpy(dst, src, n)   (void) memcpy(dst, src, n)
#define xdbd_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))

#endif  /*__ADBD__H__*/
