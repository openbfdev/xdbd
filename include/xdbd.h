#ifndef __ADBD__H__
#define __ADBD__H__


#include <bfdev/array.h>
#include "xdbd_pool.h"
#include <bfdev/allocpool.h>
#include <xdbd_config.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <bfdev/log.h>
#include <errno.h>

#define XDBD_OK 0
#define XDBD_ERR -1
#define XDBD_AGAIN      -2
#define XDBD_BUSY       -3
#define XDBD_DONE       -4
#define XDBD_DECLINED   -5
#define XDBD_ABORT      -6


#define xdbd_msec_t unsigned

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

#define xdbd_nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)

typedef ssize_t (*xdbd_recv_pt)(xdbd_connection_t *c, u_char *buf, size_t size);
typedef ssize_t (*xdbd_send_pt)(xdbd_connection_t *c, u_char *buf, size_t size);

typedef int               xdbd_err_t;

#define XDBD_EPERM         EPERM
#define XDBD_ENOENT        ENOENT
#define XDBD_ENOPATH       ENOENT
#define XDBD_ESRCH         ESRCH
#define XDBD_EINTR         EINTR
#define XDBD_ECHILD        ECHILD
#define XDBD_ENOMEM        ENOMEM
#define XDBD_EACCES        EACCES
#define XDBD_EBUSY         EBUSY
#define XDBD_EEXIST        EEXIST
#define XDBD_EEXIST_FILE   EEXIST
#define XDBD_EXDEV         EXDEV
#define XDBD_ENOTDIR       ENOTDIR
#define XDBD_EISDIR        EISDIR
#define XDBD_EINVAL        EINVAL
#define XDBD_ENFILE        ENFILE
#define XDBD_EMFILE        EMFILE
#define XDBD_ENOSPC        ENOSPC
#define XDBD_EPIPE         EPIPE
#define XDBD_EINPROGRESS   EINPROGRESS
#define XDBD_ENOPROTOOPT   ENOPROTOOPT
#define XDBD_EOPNOTSUPP    EOPNOTSUPP
#define XDBD_EADDRINUSE    EADDRINUSE
#define XDBD_ECONNABORTED  ECONNABORTED
#define XDBD_ECONNRESET    ECONNRESET
#define XDBD_ENOTCONN      ENOTCONN
#define XDBD_ETIMEDOUT     ETIMEDOUT
#define XDBD_ECONNREFUSED  ECONNREFUSED
#define XDBD_ENAMETOOLONG  ENAMETOOLONG
#define XDBD_ENETDOWN      ENETDOWN
#define XDBD_ENETUNREACH   ENETUNREACH
#define XDBD_EHOSTDOWN     EHOSTDOWN
#define XDBD_EHOSTUNREACH  EHOSTUNREACH
#define XDBD_ENOSYS        ENOSYS
#define XDBD_ECANCELED     ECANCELED
#define XDBD_EILSEQ        EILSEQ
#define XDBD_ENOMOREFILES  0
#define XDBD_ELOOP         ELOOP
#define XDBD_EBADF         EBADF
#define XDBD_EMSGSIZE      EMSGSIZE

#define XDBD_EAGAIN        EAGAIN



#define xdbd_errno                  errno
#define xdbd_socket_errno           errno
#define xdbd_set_errno(err)         errno = err
#define xdbd_set_socket_errno(err)  errno = err


#endif  /*__ADBD__H__*/
