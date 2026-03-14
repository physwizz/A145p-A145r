// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Specific feature
 *
 * Copyright (C) 2024 Samsung Electronics Co., Ltd.
 *
 * Authors:
 *  Storage Driver <storage.sec@samsung.com>
 */

#ifndef __MMC_SEC_FEATURE_H__
#define __MMC_SEC_FEATURE_H__

#include <linux/platform_device.h>

void mmc_sec_set_features(struct mmc_host *host);
void sd_sec_set_features(struct mmc_host *host);
void sd_sec_card_event(struct mmc_host *mmc);
void mmc_sd_sec_check_req_err(struct msdc_host *host, struct mmc_request *mrq);

extern struct device *sec_mmc_cmd_dev;
extern struct device *sec_sdcard_cmd_dev;
#endif
