#include <xdbd_process.h>
#include <stdlib.h>

static void xdbd_execute_proc(xdbd_t *cycle, void *data);

xdbd_pid_t xdbd_spawn_process(xdbd_t *xdbd,
    xdbd_spawn_proc_pt proc, void *data, char *name) {
    xdbd_pid_t  pid;

    pid = fork();

    switch (pid) {

    case -1:
        bfdev_log_err("fork() failed while spawning \"%s\"", name);
        // xdbd_close_channel(xdbd_processes[s].channel, cycle->log);
        return XDBD_INVALID_PID;

    case 0:
        proc(xdbd, data);
        break;

    default:
        break;
    }

    return pid;
}

xdbd_pid_t xdbd_execute(xdbd_t *cycle, xdbd_exec_ctx_t *ctx) {
    return xdbd_spawn_process(cycle, xdbd_execute_proc, ctx, ctx->name);
}


static void xdbd_execute_proc(xdbd_t *cycle, void *data) {
    xdbd_exec_ctx_t  *ctx = data;

    if (execve(ctx->path, ctx->argv, ctx->envp) == -1) {
        bfdev_log_err("execve() failed while executing %s \"%s\"",
                      ctx->name, ctx->path);
    }

    exit(1);
}
