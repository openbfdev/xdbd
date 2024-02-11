#include <bfdev/list.h>
#include <assert.h>
#include <bfdev/array.h>
#include <netinet/in.h>
#include <stdio.h>
#include <xdbd_pool.h>
#include <xdbd_event.h>
#include <xdbd.h>
#include <connection.h>


extern xdbd_event_module_t xdbd_select_module;
bfdev_list_head_t  xdbd_posted_accept_events;


static unsigned  xdbd_timer_resolution;
#define DEFAULT_CONNECTIONS  512


static void xdbd_event_accept(xdbd_event_t *ev) {
    xdbd_connection_t *lc, *c;
    xdbd_listening_t *ls;
    xdbd_sockaddr_t sa;
    socklen_t          socklen;
    xdbd_socket_t s;
    if (ev->timeout) {
        ev->timeout = 0;
    }

    ev->ready = 0;

    lc = ev->data;
    ls = lc->listening;

    socklen = sizeof(xdbd_sockaddr_t);

    s = accept(lc->fd,  &sa.sockaddr, &socklen);
    if (s == (xdbd_socket_t) -1) {
        //FIXME: add some check
        return;
    }

    c = xdbd_get_connection(ls->xdbd, s);
    if (c == NULL) {
        return;
    }

    //code: init this new c

    ls->handler(c);
    return;
}

void example_lisen_handler(xdbd_connection_t *c) {
    printf("hello world\n");
}

static xdbd_listening_t *xdbd_push_adb_default_listen(xdbd_t *xdbd) {
    xdbd_listening_t *ls;
    struct sockaddr_in *sin;

    sin = xdbd_palloc(xdbd->pool, sizeof(struct sockaddr_in));
    if (sin == NULL) {
        return NULL;
    }

    sin->sin_family = AF_INET;
    sin->sin_port = htons(ADB_CONNECTION_DEFAULT_PORT);
    sin->sin_addr.s_addr = INADDR_ANY;

    ls = xdbd_create_listening(xdbd, (struct sockaddr *)sin, sizeof(struct sockaddr_in));
    if (ls == NULL) {
        return NULL;
    }

    ls->xdbd = xdbd;
    ls->handler = example_lisen_handler;
    return ls;
}

static int xdbd_parse_listen_conf(xdbd_t *xdbd) {
    xdbd_listening_t *ls;

    //FIXME: add pool clean handler to avoid mem leak
    bfdev_array_init(&xdbd->listening, NULL, 16);

    ls = xdbd_push_adb_default_listen(xdbd);
    if (ls == NULL) {
        return XDBD_ERR;
    }

    return XDBD_OK;
}

static int xdbd_event_configration(xdbd_t *xdbd) {
    //use default value
    xdbd->connection_n = DEFAULT_CONNECTIONS;

    if (xdbd_parse_listen_conf(xdbd) != XDBD_OK) {
        return XDBD_ERR;
    }

    return XDBD_OK;
}


static int xdbd_event_init_connections(xdbd_t *xdbd) {
    xdbd_event_module_t *emodule = NULL;
    xdbd_event_t *rev, *wev;
    int i;
    xdbd_connection_t *next, *c;
    xdbd_listening_t *ls;

#if (XDBD_HAVE_SELECT)
    emodule = &xdbd_select_module;
#endif

    if (emodule == NULL) {
        //no event module support
        return XDBD_ERR;
    }

    if (emodule->actions.init(xdbd, xdbd_timer_resolution) != XDBD_OK) {
        return XDBD_ERR;
    }


    bfdev_list_head_init(&xdbd_posted_accept_events);

    xdbd->connections = xdbd_palloc(xdbd->pool, sizeof(xdbd_connection_t) * xdbd->connection_n);
    if (xdbd->connections == NULL) {
        return XDBD_ERR;
    }

    c = xdbd->connections;

    xdbd->read_events = xdbd_palloc(xdbd->pool, sizeof(xdbd_event_t) * xdbd->connection_n);
    if (xdbd->read_events == NULL) {
        return XDBD_ERR;
    }

    rev = xdbd->read_events;
    for (i = 0; i < xdbd->connection_n; i++) {
        rev[i].closed = 1;
    }

    xdbd->write_events = xdbd_palloc(xdbd->pool, sizeof(xdbd_event_t) * xdbd->connection_n);
    if (xdbd->write_events == NULL) {
        return XDBD_ERR;
    }

    wev = xdbd->write_events;
    for (i = 0; i < xdbd->connection_n; i++) {
        wev[i].closed = 1;
    }

    i = xdbd->connection_n;
    next = NULL;

    do {
        i--;

        c[i].data = next;
        c[i].read = &xdbd->read_events[i];
        c[i].write = &xdbd->write_events[i];
        c[i].fd = (xdbd_socket_t) -1;
        next = &c[i];
    } while (i);

    xdbd->free_connections = next;
    xdbd->free_connection_n = xdbd->connection_n;

    ls = bfdev_array_data(&xdbd->listening, 0);
    assert(ls != NULL);

    for (i = 0; i < bfdev_array_index(&xdbd->listening); i++) {
        c = xdbd_get_connection(xdbd, ls[i].fd);
        if (c == NULL) {
            return XDBD_ERR;
        }

        c->listening = &ls[i];
        ls[i].connection = c;

        rev = c->read;

        rev->accepted = 1;

        rev->handler = xdbd_event_accept;

        if (xdbd_add_event(rev, XDBD_READ_EVENT, 0) == XDBD_ERR) {
            return XDBD_ERR;
        }
    }

    return XDBD_OK;
}

static int xdbd_event_post_configration(xdbd_t *xdbd) {
    if (xdbd_open_listening_sockets(xdbd) != XDBD_OK) {
        return XDBD_ERR;
    }

    if (xdbd_event_init_connections(xdbd) != XDBD_OK) {
        return XDBD_ERR;
    }

    return XDBD_OK;
}

int xdbd_init_event(xdbd_t *xdbd) {
    if (xdbd_event_configration(xdbd) != XDBD_OK) {
        return XDBD_ERR;
    }

    if (xdbd_event_post_configration(xdbd) != XDBD_OK) {
        return XDBD_ERR;
    }

    return XDBD_OK;
}

void
xdbd_event_process_posted(xdbd_t *xdbd, bfdev_list_head_t *posted) {
    xdbd_event_t *ev, *tmp;


    bfdev_list_for_each_entry_safe(ev, tmp, posted, list) {
        ev->handler(ev);

        xdbd_delete_posted_event(ev);
    }
}

void xdbd_process_events_and_timers(xdbd_t *xdbd) {
    (void)xdbd_process_events(xdbd, XDBD_TIMER_INFINITE, 0);

    xdbd_event_process_posted(xdbd, &xdbd_posted_accept_events);
}
