#include <xdbd_event.h>
#include <xdbd.h>

extern xdbd_event_module_t xdbd_select_module;


static unsigned  xdbd_timer_resolution;
#define DEFAULT_CONNECTIONS  512


int xdbd_init_event(xdbd_t *xdbd) {
    xdbd_event_module_t *emodule = NULL;

    xdbd->connection_n = DEFAULT_CONNECTIONS;

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

    return XDBD_OK;
}


void xdbd_process_events_and_timers(xdbd_t *xdbd) {
    (void)xdbd_process_events(xdbd, XDBD_TIMER_INFINITE, 0);
}
