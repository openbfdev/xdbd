#include <adb.h>
#include <bfdev/log.h>
#include <xdbd_adb_request.h>
#include "xdbd_adb.h"
#include "xdbd_buf.h"
#include "xdbd_pool.h"
#include <bfdev/macro.h>
#include <xdbd.h>
#include <stdlib.h>
#include <packet.h>
#include <stdio.h>
#include <xdbd_string.h>
#include <command.h>

int xdbd_process_cnxn_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_process_auth_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_process_open_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_process_close_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_process_write_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_process_okay_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_process_bug_on_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);

static adb_pcmd_t pcmds[] = {
    {
        .pcmd =  PCMD_SYNC,
        .handler = xdbd_process_bug_on_packet
    },
    {
        .pcmd =  PCMD_CNXN,
        .handler = xdbd_process_cnxn_packet
    },
    {
        .pcmd =  PCMD_OPEN,
        .handler = xdbd_process_open_packet
    },
    {
        .pcmd =  PCMD_OKAY,
        .handler = xdbd_process_okay_packet
    },
    {
        .pcmd =  PCMD_CLSE,
        .handler = xdbd_process_close_packet
    },
    {
        .pcmd =  PCMD_WRTE,
        .handler = xdbd_process_write_packet
    },
    {
        .pcmd =  PCMD_AUTH,
        .handler = xdbd_process_auth_packet
    },
    {
        .pcmd =  PCMD_STLS,
        .handler = xdbd_process_bug_on_packet
    }
};

int xdbd_fill_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *out) {
    out->header.magic = ~out->header.pcmd;

    //TODO. compilicatable
    out->header.dchecksum = 0;


    out->header.dlen = xdbd_buf_size(out->payload);

    return XDBD_OK;
}

int xdbd_okey_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *out) {
    unsigned host_id, device_id;
    out->header.pcmd = PCMD_OKAY;

    host_id = r->p->header.arg0;
    device_id = 1;

    //the arg0 is yourself's id gen by yourself
    //the arg1 is remote's id gen by remote
    out->header.arg0 = device_id;
    out->header.arg1 = host_id;

    if (xdbd_fill_packet(r, out) != XDBD_OK) {
        return XDBD_ERR;
    }

    bfdev_log_info("prepare to send okay packet\n");
    xdbd_dump_adb_packet(r->pool, out);
    return XDBD_OK;
}

int xdbd_write_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *out, xdbd_str_t *in) {
    unsigned host_id;
    out->header.pcmd = PCMD_WRTE;

    host_id = r->p->header.arg0;

    out->header.arg0 = 1;
    out->header.arg1 = host_id;

    if (xdbd_buf_append_string(out->payload, r->pool, in) == NULL) {
        return XDBD_ERR;
    }

    if (xdbd_fill_packet(r, out) != XDBD_OK) {
        return XDBD_ERR;
    }

    bfdev_log_info("prepare to send write packet\n");

    xdbd_dump_adb_packet(r->pool, out);
    return XDBD_OK;
}

int xdbd_close_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *out) {
    unsigned host_id, device_id;
    out->header.pcmd = PCMD_CLSE;

    host_id = r->p->header.arg0;
    device_id = 1;


    out->header.arg0 = device_id;
    out->header.arg1 = host_id;

    if (xdbd_fill_packet(r, out) != XDBD_OK) {
        return XDBD_ERR;
    }

    bfdev_log_info("prepare to send okay packet\n");
    xdbd_dump_adb_packet(r->pool, out);
    return XDBD_OK;
}


int xdbd_process_cnxn_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    xdbd_adb_packet_t *out;

    out = xdbd_palloc(r->pool, sizeof(xdbd_adb_packet_t));
    if (out == NULL) {
        return XDBD_ERR;
    }

    out->payload = xdbd_create_buf(r->pool, ADB_MAX_PACKET_SIZE);
    if (out->payload == NULL) {
        return XDBD_ERR;
    }

    out->header.pcmd = PCMD_AUTH;

    out->header.arg0 = ADB_AUTH_TOKEN;
    out->header.arg1 = 0;

    //FIXME: this must be 20 size???
    xdbd_buf_append_cstring(out->payload, r->pool, "randomstring12345678");

    if (xdbd_fill_packet(r, out) != XDBD_OK) {
        return XDBD_ERR;
    }

    xdbd_dump_adb_packet(r->pool, out);

    xdbd_buf_append_mem(r->out, r->pool, &out->header, sizeof(xdbd_adb_header_t));
    xdbd_buf_append_buf(r->out, r->pool, out->payload);

    return XDBD_OK;
}


int xdbd_process_bug_on_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    bfdev_log_err("bug on: xdbd dont support this packet");
    return XDBD_ERR;
}

int xdbd_process_auth_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    bfdev_log_info("auth packet\n");
    xdbd_adb_packet_t *out;
    int i;
    //FIXME: read it from env or other place
    const char *freatures = "shell_v2,cmd,stat_v2,ls_v2,fixed_push_mkdir,\
                            apex,abb,fixed_push_symlink_timestamp,abb_exec,remount_shell,track_app,\
                            sendrecv_v2,sendrecv_v2_brotli,sendrecv_v2_lz4,\
                            sendrecv_v2_zstd,sendrecv_v2_dry_run_send,openscreen_mdns;";

    out = xdbd_palloc(r->pool, sizeof(xdbd_adb_packet_t));
    if (out == NULL) {
        return XDBD_ERR;
    }

    out->payload = xdbd_create_buf(r->pool, ADB_MAX_PACKET_SIZE);
    if (out->payload == NULL) {
        return XDBD_ERR;
    }

    out->header.pcmd = PCMD_CNXN;
    out->header.arg0 = ADB_VERSION;
    out->header.arg1 = ADB_MAX_PACKET_SIZE;

    static const char* cnxn_props[] = {
        "ro.product.name=ffashion;",
        "ro.product.model=ffashion;",
        "ro.product.device=ffashion;",
    };

    xdbd_buf_append_cstring(out->payload, r->pool, "device::");

    for (i = 0; i < BFDEV_ARRAY_SIZE(cnxn_props); i++) {
        xdbd_buf_append_cstring(out->payload, r->pool, cnxn_props[i]);
    }

    xdbd_buf_append_cstring(out->payload, r->pool, "features=");
    xdbd_buf_append_cstring(out->payload, r->pool, freatures);
    if (xdbd_fill_packet(r, out) != XDBD_OK) {
        return XDBD_ERR;
    }

    //FIXME: maybe remove it??
    xdbd_buf_append_mem(r->out, r->pool, &out->header, sizeof(xdbd_adb_header_t));
    xdbd_buf_append_buf(r->out, r->pool, out->payload);

    return XDBD_OK;
}

int xdbd_process_open_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    bfdev_log_info("open packet\n");

    if (p->header.arg0 == 0) {
        return XDBD_ERR;
    }

    return xddb_process_cmd(r, p);
}

int xdbd_process_close_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    bfdev_log_info("close packet\n");
    xdbd_adb_close_request(r);
    //FIXME: process it
    return XDBD_ERR;
}

int xdbd_process_write_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    xdbd_adb_packet_t *out;
    xdbd_connection_t *c = r->coonection;

    xdbd_str_t t_data = xdbd_string("helloworld\r\n");

    out = xdbd_palloc(r->pool, sizeof(xdbd_adb_packet_t));
    if (out == NULL) {
        return XDBD_ERR;
    }

    out->payload = xdbd_create_buf(r->pool, ADB_MAX_PACKET_SIZE);
    if (out->payload == NULL) {
        return XDBD_ERR;
    }

    xdbd_okey_packet(r, out);
    xdbd_buf_append_mem(r->out, r->pool, &out->header, sizeof(xdbd_adb_header_t));
    xdbd_buf_append_buf(r->out, r->pool, out->payload);

    c->send(c, r->out->pos, xdbd_buf_size(r->out));

    xdbd_reset_buf(out->payload);
    xdbd_reset_buf(r->out);
    xdbd_write_packet(r, out, &t_data);

    xdbd_buf_append_mem(r->out, r->pool, &out->header, sizeof(xdbd_adb_header_t));
    xdbd_buf_append_buf(r->out, r->pool, out->payload);

    c->send(c, r->out->pos, xdbd_buf_size(r->out));
    xdbd_reset_buf(r->out);

    return XDBD_OK;
}

int xdbd_process_okay_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    xdbd_adb_packet_t *out;

    out = xdbd_palloc(r->pool, sizeof(xdbd_adb_packet_t));
    if (out == NULL) {
        return XDBD_ERR;
    }

    out->payload = xdbd_create_buf(r->pool, ADB_MAX_PACKET_SIZE);
    if (out->payload == NULL) {
        return XDBD_ERR;
    }

    xdbd_okey_packet(r, out);
    xdbd_buf_append_mem(r->out, r->pool, &out->header, sizeof(xdbd_adb_header_t));
    xdbd_buf_append_buf(r->out, r->pool, out->payload);

    return XDBD_OK;
}

int xdbd_process_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p) {
    xdbd_adb_header_t *h;
    adb_pcmd_t *pcmd;
    h = &p->header;
    size_t i;

    if (h->magic != ~h->pcmd) {
        bfdev_log_err("process_packet: magic error\n");
        return XDBD_ERR;
    }

    if (r->out == NULL) {
        r->out = xdbd_create_buf(r->pool, 1024);
        if (r->out == NULL) {
            return XDBD_ERR;
        }
    }

    for (i = 0; i < BFDEV_ARRAY_SIZE(pcmds); i++) {
        pcmd = &pcmds[i];

        if (pcmd->pcmd != h->pcmd) {
            continue;
        }

        return pcmd->handler(r, p);
    }

    return XDBD_ERR;
}

xdbd_buf_t *xdbd_dump_hex(xdbd_pool_t *pool, xdbd_str_t str) {
    xdbd_buf_t *b;
    size_t i;
    char c[3];
    b = xdbd_create_buf(pool, 1024);
    if (b == NULL) {
        return NULL;
    }

    for (i = 0; i < str.size; i++) {
        snprintf(c, sizeof(c), "%02x", str.data[i]);
        if (xdbd_buf_append_cstring(b, pool, c) == NULL) {
            return NULL;
        }
    }

    return b;
}

xdbd_buf_t *xdbd_dump_adb_header(xdbd_pool_t *pool, const xdbd_adb_header_t *h) {
    unsigned command = h->pcmd;
    int len = h->dlen;
    char cmd[9];
    char arg0[12], arg1[12];
    int n;

    for (n = 0; n < 4; n++) {
        int b = (command >> (n * 8)) & 255;
        if (b < 32 || b >= 127) break;
        cmd[n] = (char)b;
    }
    if (n == 4) {
        cmd[4] = 0;
    } else {
        // There is some non-ASCII name in the command, so dump the hexadecimal value instead
        snprintf(cmd, sizeof cmd, "%08x", command);
    }

    if (h->arg0 < 256U)
        snprintf(arg0, sizeof arg0, "%d", h->arg0);
    else
        snprintf(arg0, sizeof arg0, "0x%x", h->arg0);

    if (h->arg1 < 256U)
        snprintf(arg1, sizeof arg1, "%d", h->arg1);
    else
        snprintf(arg1, sizeof arg1, "0x%x", h->arg1);


    bfdev_log_debug("[%s] arg0=%s arg1=%s (len=%d), (magic=%u)\n", cmd, arg0, arg1, len, h->magic);

    return NULL;
}

void xdbd_dump_adb_packet(xdbd_pool_t *pool, const xdbd_adb_packet_t *p) {
    //FIXME:
    xdbd_str_t hex_payload;
    xdbd_buf_t *payload;

    xdbd_dump_adb_header(pool, &p->header);

    hex_payload.data = p->payload->pos;
    hex_payload.size = xdbd_buf_size(p->payload);

    payload = xdbd_dump_hex(pool, hex_payload);

    bfdev_log_debug("%.*s\n", (int)xdbd_buf_size(payload), payload->pos);
    return;
}
