/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
 */

#ifndef __XDBD_TIMER__H__
#define __XDBD_TIMER__H__

#include <xdbd.h>
#include <bfdev.h>

struct xdbd_timer_s {
    bfdev_rb_node_t rb;
    xdbd_msec_t time;
};

extern bfdev_rb_root_cached_t xdbd_timers;

extern xdbd_timer_t *
xdbd_first_timer(void);

extern void
xdbd_add_timer(xdbd_timer_t *timer);

extern void
xdbd_remove_timer(xdbd_timer_t *timer);

#endif /* __XDBD_TIMER__H__ */
