#ifndef __XDBD_EVENT__H__
#define __XDBD_EVENT__H__
#include "bfdev/list.h"
#include <xdbd.h>

#define XDBD_INVALID_INDEX  0xd0d0d0d0

typedef void (*xdbd_event_handler_pt)(xdbd_event_t *ev);

struct xdbd_event_s {
    void *data;
    int index;
    xdbd_event_handler_pt handler;

    bfdev_list_head_t list;
    unsigned write:1;
    unsigned ready:1;
    unsigned closed:1;
    unsigned accepted:1;
    unsigned posted:1;
    unsigned timeout:1;
    unsigned active:1;
    unsigned eof:1;
    unsigned error:1;
};

typedef struct xdbd_event_actions_s {
    int  (*add)(xdbd_event_t *ev, int event, unsigned flags);
    int  (*del)(xdbd_event_t *ev, int event, unsigned flags);

    int  (*enable)(xdbd_event_t *ev, int event, unsigned flags);
    int  (*disable)(xdbd_event_t *ev, int event, unsigned flags);

    int  (*add_conn)(xdbd_connection_t *c);
    int  (*del_conn)(xdbd_connection_t *c, unsigned flags);

    int  (*notify)(xdbd_event_handler_pt handler);

    int  (*process_events)(xdbd_t *xdbd, xdbd_msec_t timer,
                                 unsigned flags);
    int  (*init)(xdbd_t *xdbd, xdbd_msec_t timer);
    void (*done)(xdbd_t *xdbd);
} xdbd_event_actions_t;


#define XDBD_USE_LEVEL_EVENT      0x00000001

#define XDBD_TIMER_INFINITE  (xdbd_msec_t) -1

extern xdbd_event_actions_t xdbd_event_actions;
extern bfdev_list_head_t  xdbd_posted_accept_events;
extern bfdev_list_head_t  xdbd_posted_events;

#define xdbd_post_event(ev, list)                                                 \
                                                                                  \
    if (!(ev)->posted) {                                                          \
        (ev)->posted = 1;                                                         \
        bfdev_list_add_prev(list, &(ev)->list);                               \
    }

#define xdbd_delete_posted_event(ev)                                           \
                                                                              \
    (ev)->posted = 0;                                                         \
    bfdev_list_del(&(ev)->list);


#if (XDBD_HAVE_EPOLL) && !(XDBD_HAVE_EPOLLRDHUP)
#define EPOLLRDHUP         0
#endif


#if (XDBD_HAVE_KQUEUE)

#define XDBD_READ_EVENT     EVFILT_READ
#define XDBD_WRITE_EVENT    EVFILT_WRITE

#undef  XDBD_VNODE_EVENT
#define XDBD_VNODE_EVENT    EVFILT_VNODE

/*
 * XDBD_CLOSE_EVENT, XDBD_LOWAT_EVENT, and XDBD_FLUSH_EVENT are the module flags
 * and they must not go into a kernel so we need to choose the value
 * that must not interfere with any existent and future kqueue flags.
 * kqueue has such values - EV_FLAG1, EV_EOF, and EV_ERROR:
 * they are reserved and cleared on a kernel entrance.
 */
#undef  XDBD_CLOSE_EVENT
#define XDBD_CLOSE_EVENT    EV_EOF

#undef  XDBD_LOWAT_EVENT
#define XDBD_LOWAT_EVENT    EV_FLAG1

#undef  XDBD_FLUSH_EVENT
#define XDBD_FLUSH_EVENT    EV_ERROR

#define XDBD_LEVEL_EVENT    0
#define XDBD_ONESHOT_EVENT  EV_ONESHOT
#define XDBD_CLEAR_EVENT    EV_CLEAR

#undef  XDBD_DISABLE_EVENT
#define XDBD_DISABLE_EVENT  EV_DISABLE


#elif (XDBD_HAVE_DEVPOLL && !(XDBD_TEST_BUILD_DEVPOLL)) \
      || (XDBD_HAVE_EVENTPORT && !(XDBD_TEST_BUILD_EVENTPORT))

#define XDBD_READ_EVENT     POLLIN
#define XDBD_WRITE_EVENT    POLLOUT

#define XDBD_LEVEL_EVENT    0
#define XDBD_ONESHOT_EVENT  1


#elif (XDBD_HAVE_EPOLL) && !(XDBD_TEST_BUILD_EPOLL)

#define XDBD_READ_EVENT     (EPOLLIN|EPOLLRDHUP)
#define XDBD_WRITE_EVENT    EPOLLOUT

#define XDBD_LEVEL_EVENT    0
#define XDBD_CLEAR_EVENT    EPOLLET
#define XDBD_ONESHOT_EVENT  0x70000000
#if 0
#define XDBD_ONESHOT_EVENT  EPOLLONESHOT
#endif

#if (XDBD_HAVE_EPOLLEXCLUSIVE)
#define XDBD_EXCLUSIVE_EVENT  EPOLLEXCLUSIVE
#endif

#elif (XDBD_HAVE_POLL)

#define XDBD_READ_EVENT     POLLIN
#define XDBD_WRITE_EVENT    POLLOUT

#define XDBD_LEVEL_EVENT    0
#define XDBD_ONESHOT_EVENT  1


#else /* select */

#define XDBD_READ_EVENT     0
#define XDBD_WRITE_EVENT    1

#define XDBD_LEVEL_EVENT    0
#define XDBD_ONESHOT_EVENT  1

#endif /* XDBD_HAVE_KQUEUE */

/*
 * The event filter is deleted just before the closing file.
 * Has no meaning for select and poll.
 * kqueue, epoll, eventport:         allows to avoid explicit delete,
 *                                   because filter automatically is deleted
 *                                   on file close,
 *
 * /dev/poll:                        we need to flush POLLREMOVE event
 *                                   before closing file.
 */
#define XDBD_CLOSE_EVENT    1

/*
 * disable temporarily event filter, this may avoid locks
 * in kernel malloc()/free(): kqueue.
 */
#define XDBD_DISABLE_EVENT  2

/*
 * event must be passed to kernel right now, do not wait until batch processing.
 */
#define XDBD_FLUSH_EVENT    4


/* these flags have a meaning only for kqueue */
#define XDBD_LOWAT_EVENT    0
#define XDBD_VNODE_EVENT    0



#define xdbd_process_events   xdbd_event_actions.process_events
#define xdbd_done_events      xdbd_event_actions.done

#define xdbd_add_event        xdbd_event_actions.add
#define xdbd_del_event        xdbd_event_actions.del
#define xdbd_add_conn         xdbd_event_actions.add_conn
#define xdbd_del_conn         xdbd_event_actions.del_conn


typedef struct xdbd_event_module_s {
    char *name;
    xdbd_event_actions_t actions;
} xdbd_event_module_t;


int xdbd_init_event(xdbd_t *xdbd);
void xdbd_process_events_and_timers(xdbd_t *xdbd);

extern unsigned              xdbd_event_flags;
extern xdbd_event_actions_t  xdbd_event_actions;

#endif  /*__XDBD_EVENT__H__*/
