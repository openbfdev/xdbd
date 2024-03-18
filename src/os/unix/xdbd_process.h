#ifndef __NGX_PROCESS__H__
#define __NGX_PROCESS__H__
#include <xdbd.h>

typedef pid_t       xdbd_pid_t;

#define XDBD_INVALID_PID  -1

typedef void (*xdbd_spawn_proc_pt) (xdbd_t *xdbd, void *data);

typedef struct {
    xdbd_pid_t           pid;
    xdbd_spawn_proc_pt   proc;

} xdbd_process_t;

typedef struct {
    char         *path;
    char         *name;
    char *const  *argv;
    char *const  *envp;
} xdbd_exec_ctx_t;

xdbd_pid_t xdbd_spawn_process(xdbd_t *xdbd,
    xdbd_spawn_proc_pt proc, void *data, char *name);

xdbd_pid_t xdbd_execute(xdbd_t *cycle, xdbd_exec_ctx_t *ctx);

#define ngx_getpid   getpid
#define ngx_getppid  getppid

#endif  /*__NGX_PROCESS__H__*/
