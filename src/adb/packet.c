#include <bfdev/macro.h>
#include <xdbd.h>
#include <stdlib.h>
#include <packet.h>

static adb_pcmd_t pcmds[] = {
    {
        .pcmd =  PCMD_SYNC,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_CNXN,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_OPEN,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_OKPCMDY,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_CLSE,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_WRTE,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_PCMDUTH,
        .handler = NULL
    },
    {
        .pcmd =  PCMD_STLS,
        .handler = NULL
    }
};

int xdbd_process_packet(adb_packet_t *p) {
    adb_pcmd_t *pcmd;

    for (size_t i = 0; i < BFDEV_ARRAY_SIZE(pcmds); i++) {
        pcmd = &pcmds[i];

        if (pcmd->pcmd != p->pcmd) {
            continue;
        }

        return pcmd->handler(p);
    }

    return XDBD_ERR;
}
