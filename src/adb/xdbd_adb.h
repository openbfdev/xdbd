#ifndef __XDBD_ADB__H__
#define __XDBD_ADB__H__

#include <xdbd.h>
#include <connection.h>
#include <xdbd_event.h>
#include <xdbd_adb_request.h>
#define ADB_MAX_PACKET_SIZE 1024 * 1024

void xdbd_adb_wait_apacket_handler(xdbd_event_t *ev);
void xdbd_adb_empty_handler(xdbd_event_t *ev);
void xdbd_adb_block_reading(xdbd_event_t *ev);
void xdbd_adb_close_connection(xdbd_connection_t *c);
void xdbd_adb_close_request(xdbd_adb_request_t *r);
#endif  /*__XDBD_ADB__H__*/
