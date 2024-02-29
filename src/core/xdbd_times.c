/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
 */

#include <bfdev.h>
#include <xdbd.h>
#include <time.h>
#include <sys/time.h>

volatile xdbd_msec_t      xdbd_current_msec;

static xdbd_msec_t
xdbd_monotonic_time(void)
{
    struct timespec  ts;
    time_t sec;
    long msec;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    sec = ts.tv_sec;
    msec = ts.tv_nsec / 1000000;

    return (xdbd_msec_t) sec * 1000 + msec;
}

void
xdbd_time_update(void)
{
    xdbd_current_msec = xdbd_monotonic_time();
}
