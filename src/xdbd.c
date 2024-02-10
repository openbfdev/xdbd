#include <xdbd_event.h>
#include <bfdev/list.h>
#include <bfdev.h>
#include <xdbd.h>
#include <connection.h>
#include <xdbd_pool.h>

static void usage() {

}

static int xdbd_get_options(int argc, char *const *argv) {
    //FIXME: get options
    return XDBD_OK;
}

static int xdbd_worker_init(xdbd_t *xdbd) {
    return XDBD_OK;
}

static xdbd_t *xdbd_init() {
    xdbd_t *xdbd = NULL;
    xdbd_pool_t *pool;

    pool = xdbd_create_pool();
    if (pool == NULL) {
        return NULL;
    }

    xdbd = xdbd_palloc(pool, sizeof(xdbd_t));
    if (xdbd == NULL) {
        xdbd_destroy_pool(pool);
        return NULL;
    }

    xdbd->pool = pool;

    if (xdbd_init_event(xdbd) != XDBD_OK) {
        xdbd_destroy_pool(pool);
        return NULL;
    }


    return xdbd;
}

static void xdbd_worker_process(xdbd_t *xdbd) {
    for (;;) {
        //TODO: process the signal
        xdbd_process_events_and_timers(xdbd);
    }
}

int	main(int argc, char **argv) {
    xdbd_t *xdbd;

    if (xdbd_get_options(argc, argv) != XDBD_OK) {
        return XDBD_ERR;
    }

    xdbd = xdbd_init();
    if (xdbd == NULL) {
        return XDBD_ERR;
    }

    xdbd_worker_process(xdbd);

    return 0;
}
