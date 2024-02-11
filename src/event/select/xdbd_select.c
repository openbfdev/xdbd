#include "bfdev/log.h"
#include <stdlib.h>
#include <bfdev/allocator.h>
#include <assert.h>
#include <sys/select.h>
#include <xdbd.h>
#include <xdbd_event.h>
#include <connection.h>
#include <sys/errno.h>

static int xdbd_select_init(xdbd_t *cycle, xdbd_msec_t timer);
static int xdbd_select_add_event(xdbd_event_t *ev, int event, unsigned flags);
static int xdbd_select_del_event(xdbd_event_t *ev, int event, unsigned flags);
static int xdbd_select_process_events(xdbd_t *xdbd, xdbd_msec_t timer, unsigned flags);
static void xdbd_select_done(xdbd_t *xdbd);

static fd_set         master_read_fd_set;
static fd_set         master_write_fd_set;
static fd_set         work_read_fd_set;
static fd_set         work_write_fd_set;

static int          max_fd;
static unsigned     nevents;

static xdbd_event_t  **event_index;

xdbd_event_module_t xdbd_select_module = {
    .name = "select",
    .actions =
    {
        .add = xdbd_select_add_event,
        .del = xdbd_select_del_event,
        .enable = xdbd_select_add_event,
        .disable = xdbd_select_del_event,
        .add_conn = NULL,
        .del_conn = NULL,
        .notify = NULL,
        .process_events = xdbd_select_process_events,
        .init = xdbd_select_init,
        .done = xdbd_select_done,
    },
};


static int xdbd_select_init(xdbd_t *xdbd, xdbd_msec_t timer) {
    assert(event_index == NULL);

    FD_ZERO(&master_read_fd_set);
    FD_ZERO(&master_write_fd_set);
    assert(nevents == 0);

    /*double connections means read and write */
    event_index = bfdev_malloc(NULL, sizeof(xdbd_event_t *) * xdbd->connection_n * 2);
    if (event_index == NULL) {
        return XDBD_ERR;
    }

    xdbd_event_flags = XDBD_USE_LEVEL_EVENT;
    max_fd = -1;

    //register this module.
    xdbd_event_actions = xdbd_select_module.actions;
    return XDBD_OK;
}

static int xdbd_select_add_event(xdbd_event_t *ev, int event, unsigned flags) {
    xdbd_connection_t *c;

    c = ev->data;
    if (ev->index != XDBD_INVALID_INDEX) {
        //this event allready in
        return XDBD_OK;
    }

    if (event == XDBD_READ_EVENT) {
        FD_SET(c->fd, &master_read_fd_set);
    }

    if (event == XDBD_WRITE_EVENT) {
        FD_SET(c->fd, &master_write_fd_set);
    }

    if (max_fd != -1 && max_fd < c->fd) {
        max_fd = c->fd;
    }

    /*save this event to event_index*/
    event_index[nevents] = ev;
    ev->index = nevents;
    nevents++;

    return XDBD_OK;
}

static int xdbd_select_del_event(xdbd_event_t *ev, int event, unsigned flags) {
    xdbd_connection_t *c;
    xdbd_event_t *e;
    c = ev->data;

    if (ev->index == XDBD_INVALID_INDEX) {
        return XDBD_ERR;
    }

    if (event == XDBD_READ_EVENT) {
        FD_CLR(c->fd, &master_read_fd_set);
    }

    if (event == XDBD_WRITE_EVENT) {
        FD_CLR(c->fd, &master_write_fd_set);
    }

    if (c->fd == max_fd) {
        max_fd = -1;
    }

    if (ev->index < --nevents) {
        e = event_index[nevents];
        event_index[ev->index] = e;
        e->index = ev->index;
    }

    ev->index = XDBD_INVALID_INDEX;

    return XDBD_OK;
}

static int xdbd_select_process_events(xdbd_t *xdbd, xdbd_msec_t timer, unsigned flags) {
    xdbd_connection_t *c;
    int i = 0, ready, nready, err, found;
    bfdev_list_head_t *list;
    /* struct timeval     tv, *tp; */
    xdbd_event_t *ev;
    if (max_fd == -1) {
        for (i = 0; i < nevents; i++) {
            c = event_index[i]->data;
            if (max_fd < c->fd) {
                max_fd = c->fd;
            }
        }

        //change max_fd
    }

    work_read_fd_set = master_read_fd_set;
    work_write_fd_set = master_write_fd_set;

    ready = select(max_fd + 1, &work_read_fd_set, &work_write_fd_set, NULL, NULL);

    err = (ready == -1) ? errno : 0;

    if (err) {
        //process something. now skip it
    }

    if (ready == 0) {
        if (timer != XDBD_TIMER_INFINITE) {
            return XDBD_OK;
        }

        return XDBD_ERR;
    }

    nready = 0;

    for (i = 0; i < nevents; i++) {
        ev = event_index[i];
        c = ev->data;

        found = 0;

        if (ev->write) {
            if (FD_ISSET(c->fd, &work_write_fd_set)) {
                found = 1;
            }
        }

        /*read event*/
        if (!ev->write) {
            if (FD_ISSET(c->fd, &work_read_fd_set)) {
                found = 1;
            }
        }

        if (found) {
            ev->ready = 1;

            list = ev->accepted ? &xdbd_posted_accept_events : &xdbd_posted_events;

            xdbd_post_event(ev,  list);
            nready++;
        }
    }

    if (ready != nready) {
        //TODO: need fix
        bfdev_log_warn("ready != nready");
        return XDBD_ERR;
    }
    return XDBD_OK;
}

static void xdbd_select_done(xdbd_t *xdbd) {
    bfdev_free(NULL, event_index);
    event_index = NULL;
    return ;
}
