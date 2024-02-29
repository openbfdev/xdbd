/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
 */

#include <xdbd_timer.h>

BFDEV_RB_ROOT_CACHED(xdbd_timers);

static inline long
timer_cmp(const bfdev_rb_node_t *key1, const bfdev_rb_node_t *key2, void *pdata)
{
    xdbd_timer_t *node1, *node2;

    node1 = bfdev_container_of(key1, xdbd_timer_t, rb);
    node2 = bfdev_container_of(key2, xdbd_timer_t, rb);

    /* Ignoring conflicts */
    return node1->time < node2->time ? BFDEV_LT : BFDEV_BT;
}

xdbd_timer_t *
xdbd_first_timer(void)
{
    return bfdev_rb_cached_first_entry(&xdbd_timers, xdbd_timer_t, rb);
}

void
xdbd_add_timer(xdbd_timer_t *timer)
{
    bfdev_rb_cached_insert(&xdbd_timers, &timer->rb, timer_cmp, NULL);
}

void
xdbd_remove_timer(xdbd_timer_t *timer)
{
    bfdev_rb_cached_remove(&xdbd_timers, &timer->rb);
}
