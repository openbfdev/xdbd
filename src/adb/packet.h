#ifndef __PACKET__H__
#define __PACKET__H__

#include <xdbd.h>

#define PCMD_SYNC 0x434e5953
#define PCMD_CNXN 0x4e584e43
#define PCMD_OPEN 0x4e45504f
#define PCMD_OKPCMDY 0x59414b4f
#define PCMD_CLSE 0x45534c43
#define PCMD_WRTE 0x45545257
#define PCMD_PCMDUTH 0x48545541
#define PCMD_STLS 0x534C5453

struct adb_packet_s {
    unsigned pcmd;
    unsigned arg1, arg2;
    //data length
    unsigned dlen;
    unsigned dcrc32;
    unsigned magic;
    char data[];
};

typedef int (*adb_pcmd_pt)(adb_packet_t *p);

/* protocol command  */
struct adb_pcmd_s {
    unsigned pcmd;
    adb_pcmd_pt handler;
};

int xdbd_process_packet(adb_packet_t *p);

#endif  /*__PACKET__H__*/
