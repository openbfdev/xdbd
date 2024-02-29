#ifndef __PACKET__H__
#define __PACKET__H__

#include "xdbd_string.h"
#include <xdbd.h>
#include <adb.h>
#include <bfdev.h>

/*
#define PCMD_SYNC 0x434e5953
#define PCMD_CNXN 0x4e584e43
#define PCMD_OPEN 0x4e45504f
#define PCMD_OKAY 0x59414b4f
#define PCMD_CLSE 0x45534c43
#define PCMD_WRTE 0x45545257
#define PCMD_AUTH 0x48545541
#define PCMD_STLS 0x534C5453
*/

#define PCMD_ASCII(pcmd) (       \
    (((pcmd[3]) & 0xff) << 24) | \
    (((pcmd[2]) & 0xff) << 16) | \
    (((pcmd[1]) & 0xff) <<  8) | \
    (((pcmd[0]) & 0xff) <<  0)   \
)

#define PCMD_SYNC PCMD_ASCII("SYNC")
#define PCMD_CNXN PCMD_ASCII("CNXN")
#define PCMD_OPEN PCMD_ASCII("OPEN")
#define PCMD_OKAY PCMD_ASCII("OKAY")
#define PCMD_CLSE PCMD_ASCII("CLSE")
#define PCMD_WRTE PCMD_ASCII("WRTE")
#define PCMD_AUTH PCMD_ASCII("AUTH")
#define PCMD_STLS PCMD_ASCII("STLS")


struct xdbd_adb_header_s {
    unsigned pcmd;
    unsigned arg0, arg1;
    //data length
    unsigned dlen;
    unsigned dchecksum;
    unsigned magic;
} __bfdev_packed;

struct xdbd_adb_packet_s {
    xdbd_adb_header_t header;
    xdbd_buf_t *payload;
};

typedef int (*xdbd_adb_pcmd_pt)(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);

/* protocol command  */
struct adb_pcmd_s {
    unsigned pcmd;
    xdbd_adb_pcmd_pt handler;
};

int xdbd_process_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
void xdbd_dump_adb_packet(xdbd_pool_t *pool, const xdbd_adb_packet_t *p);

int xdbd_okey_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p);
int xdbd_write_packet(xdbd_adb_request_t *r, xdbd_adb_packet_t *p, xdbd_str_t *in);
#endif  /*__PACKET__H__*/
