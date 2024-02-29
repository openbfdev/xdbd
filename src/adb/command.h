#ifndef __COMMAND__H__
#define __COMMAND__H__

#include <xdbd_string.h>
#include <adb.h>

typedef int (*adb_cmd_pt)(xdbd_adb_request_t *r, xdbd_adb_packet_t *p, const adb_command_t *cmd);


struct adb_command_s {
    xdbd_str_t cmd;
    adb_cmd_pt handler;
};

int xddb_process_cmd(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);

#endif  /*__COMMAND__H__*/
