#include <xdbd.h>
#include <command.h>
#include <stdlib.h>

adb_command_t adb_cmds[] = {
    {
        .cmd = "shell",
        .handler = NULL,
    },
};
