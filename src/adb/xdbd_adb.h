#ifndef __XDBD_ADB__H__
#define __XDBD_ADB__H__

#include <xdbd.h>
#include <connection.h>
#include <xdbd_event.h>


void xdbd_adb_wait_apacket_handler(xdbd_event_t *ev);
void xdbd_adb_empty_handler(xdbd_event_t *ev);
void xdbd_adb_close_connection(xdbd_connection_t *c);
#endif  /*__XDBD_ADB__H__*/
