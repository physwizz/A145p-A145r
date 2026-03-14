/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef WAKEUP_MOTION_H
#define WAKEUP_MOTION_H

#include <linux/ioctl.h>
#include <linux/init.h>

int __init wakeup_motion_init(void);
void __exit wakeup_motion_exit(void);

#endif
