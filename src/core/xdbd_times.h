/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
 */

#ifndef __xdbd_times__H__
#define __xdbd_times__H__

#include <xdbd.h>

extern volatile xdbd_msec_t      xdbd_current_msec;

void
xdbd_time_update(void);

#endif /* __xdbd_times__H__ */
