#include <adb.h>
#include <bfdev/macro.h>
#include <string.h>
#include <xdbd.h>
#include <command.h>
#include <packet.h>
#include <xdbd_buf.h>
#include <xdbd_adb.h>
#include <xdbd_string.h>

int xdbd_process_shell_cmd(xdbd_adb_request_t *r, xdbd_adb_packet_t *p, const adb_command_t *cmd);

static adb_command_t adb_cmds[] = {
    {
        .cmd = xdbd_string("shell"),
        .handler = xdbd_process_shell_cmd,
    },
};

int xdbd_process_shell_cmd(xdbd_adb_request_t *r, xdbd_adb_packet_t *p, const adb_command_t *cmd) {
    xdbd_adb_packet_t *out;

    out = xdbd_palloc(r->pool, sizeof(xdbd_adb_packet_t));
    if (out == NULL) {
        return XDBD_ERR;
    }

    out->payload = xdbd_create_buf(r->pool, ADB_MAX_PACKET_SIZE);
    if (out->payload == NULL) {
        return XDBD_ERR;
    }

    bfdev_log_info("shell_cmd: send okay packet\n");
    xdbd_okey_packet(r, out);

    xdbd_buf_append_mem(r->out, r->pool, &out->header, sizeof(xdbd_adb_header_t));
    xdbd_buf_append_buf(r->out, r->pool, out->payload);

    return XDBD_OK;
}

int xddb_process_cmd(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    size_t i;
    const xdbd_adb_header_t *h;
    const adb_command_t *cmd;
    const xdbd_buf_t *paylod;

    h = &p->header;
    paylod = p->payload;
    for (i = 0; i < BFDEV_ARRAY_SIZE(adb_cmds); i++) {
        cmd = &adb_cmds[i];

        /*FIME: parse the payload to cmd and argv*/
    /*
        if (cmd->cmd.size != h->dlen) {
            continue;
        }

        if (strncmp((const char *)cmd->cmd.data, (const char *)paylod->pos, h->dlen) != 0) {
            continue;
        }
    */
        return cmd->handler(r, p, cmd);
    }

    return XDBD_ERR;
}
