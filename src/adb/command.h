#ifndef __COMMAND__H__
#define __COMMAND__H__

#include <adb.h>

typedef int (*adb_cmd_pt)(const char *cmd);


struct adb_command_s {
    const char *cmd;
    adb_cmd_pt handler;
};


#endif  /*__COMMAND__H__*/
