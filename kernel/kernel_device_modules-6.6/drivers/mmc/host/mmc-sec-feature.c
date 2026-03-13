// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Specific feature
 *
 * Copyright (C) 2024 Samsung Electronics Co., Ltd.
 *
 * Authors:
 *  Storage Driver <storage.sec@samsung.com>
 */

#include <linux/sched/clock.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>

#include "mtk-mmc.h"
#include "mmc-sec-feature.h"
#include "mmc-sec-sysfs.h"

struct mmc_sd_sec_device_info mdi;
struct mmc_sd_sec_device_info sdi;

static void mmc_sd_sec_inc_err_count(struct mmc_sd_sec_err_info *err_log,
		int index, int error, u32 status)
{
	int i = 0;
	int cpu = raw_smp_processor_id();

	if (!error)
		return;

	/*
	 * -EIO (-5) : SDMMC_INT_RESP_ERR error case. So log as CRC.
	 * -ENOMEDIUM (-123), etc : SW timeout and other error. So log as TIMEOUT.
	 */
	switch (error) {
	case -EIO:
		error = -EILSEQ;
		break;
	case -EILSEQ:
		break;
	default:
		error = -ETIMEDOUT;
		break;
	}

	for (i = 0; i < MAX_ERR_TYPE_INDEX; i++) {
		if (err_log[index + i].err_type == error) {
			index += i;
			break;
		}
	}

	if (i >= MAX_ERR_TYPE_INDEX)
		return;

	/* log device status and time if this is the first error  */
	if (!err_log[index].status || !(R1_CURRENT_STATE(status) & R1_STATE_TRAN))
		err_log[index].status = status;
	if (!err_log[index].first_issue_time)
		err_log[index].first_issue_time = cpu_clock(cpu);
	err_log[index].last_issue_time = cpu_clock(cpu);
	err_log[index].count++;
}

static void mmc_sd_sec_inc_status_err(struct mmc_sd_sec_status_err_info *status_err,
		struct mmc_card *card, u32 status)
{
	if (status & R1_ERROR)
		status_err->ge_cnt++;

	if (status & R1_CC_ERROR)
		status_err->cc_cnt++;

	if (status & R1_CARD_ECC_FAILED)
		status_err->ecc_cnt++;

	if (status & R1_WP_VIOLATION)
		status_err->wp_cnt++;

	if (status & R1_OUT_OF_RANGE)
		status_err->oor_cnt++;
}

#define MMC_BLK_TIMEOUT_MS (9 * 1000)
static bool mmc_sd_sec_check_busy_stuck(struct mmc_sd_sec_device_info *cdi,
		u32 status)
{
	if (time_before(jiffies,
		cdi->tstamp_last_cmd + msecs_to_jiffies(MMC_BLK_TIMEOUT_MS)))
		return false;

	if (status && (!(status & R1_READY_FOR_DATA) ||
			(R1_CURRENT_STATE(status) == R1_STATE_PRG)))
		return true;

	return false;
}

static void mmc_sd_sec_log_err_count(struct mmc_sd_sec_device_info *cdi,
		struct mmc_host *mmc, struct mmc_request *mrq)
{
	u32 status = (mrq->sbc ? mrq->sbc->resp[0] : 0) |
				(mrq->stop ? mrq->stop->resp[0] : 0) |
				(mrq->cmd ? mrq->cmd->resp[0] : 0);

	if (status & STATUS_MASK)
		mmc_sd_sec_inc_status_err(&cdi->status_err, mmc->card, status);

	if (mrq->cmd->error)
		mmc_sd_sec_inc_err_count(&cdi->err_info[0],
			MMC_SD_CMD_OFFSET, mrq->cmd->error, status);
	if (mrq->sbc && mrq->sbc->error)
		mmc_sd_sec_inc_err_count(&cdi->err_info[0],
			MMC_SD_SBC_OFFSET, mrq->sbc->error, status);
	if (mrq->data && mrq->data->error)
		mmc_sd_sec_inc_err_count(&cdi->err_info[0],
			MMC_SD_DATA_OFFSET, mrq->data->error, status);
	if (mrq->stop && mrq->stop->error)
		mmc_sd_sec_inc_err_count(&cdi->err_info[0],
			MMC_SD_STOP_OFFSET, mrq->stop->error, status);

	/*
	 * Core driver polls card busy for 10s, MMC_BLK_TIMEOUT_MS.
	 * If card status is still in prog state after 9s by cmd13
	 * and tstamp_last_cmd has not been updated by next cmd,
	 * log as write busy timeout.
	 */
	if (mrq->cmd->opcode != MMC_SEND_STATUS)
		return;

	if (mmc_sd_sec_check_busy_stuck(cdi, status)) {
		/* card stuck in prg state */
		mmc_sd_sec_inc_err_count(&cdi->err_info[0], MMC_SD_BUSY_OFFSET, -ETIMEDOUT, status);
		/* not to check card busy again */
		cdi->tstamp_last_cmd = jiffies;
	}
}

static bool mmc_sd_sec_check_cmd_type(struct mmc_request *mrq)
{
	/*
	 * cmd->flags info
	 * MMC_CMD_AC	 (0b00 << 5) : Addressed commands
	 * MMC_CMD_ADTC  (0b01 << 5) : Addressed data transfer commands
	 * MMC_CMD_BC	 (0b10 << 5) : Broadcast commands
	 * MMC_CMD_BCR	 (0b11 << 5) : Broadcast commands with response
	 *
	 * Log the errors only for AC or ADTC type
	 */
	if (!(mrq->cmd->flags & MMC_RSP_PRESENT))
		return false;

	if (mrq->cmd->flags & MMC_CMD_BC)
		return false;

	/*
	 * No need to check if MMC_RSP_136 set or cmd MMC_APP_CMD.
	 * CMD55 is sent with MMC_CMD_AC flag but no need to log.
	 */
	if ((mrq->cmd->flags & MMC_RSP_136) ||
			(mrq->cmd->opcode == MMC_APP_CMD))
		return false;

	return true;
}

void mmc_sd_sec_check_req_err(struct msdc_host *host, struct mmc_request *mrq)
{
	struct mmc_sd_sec_device_info *cdi;
	struct mmc_host *mmc = mmc_from_priv(host);

	if (!mmc->card || !mrq || !mrq->cmd)
		return;

	cdi = get_device_info(mmc);

	/* Return if the cmd is tuning block */
	if ((mrq->cmd->opcode == MMC_SEND_TUNING_BLOCK) ||
			(mrq->cmd->opcode == MMC_SEND_TUNING_BLOCK_HS200))
		return;

	/* Return if the cmd is send ext_csd and called by tuning process */
	if ((mrq->cmd->opcode ==  MMC_SEND_EXT_CSD) && (host->hs400_tuning == true))
		return;

	/* Set CMD(except CMD13) timestamp to check card stuck */
	if (mrq->cmd->opcode != MMC_SEND_STATUS)
		cdi->tstamp_last_cmd = jiffies;

	if (mmc_sd_sec_check_cmd_type(mrq))
		mmc_sd_sec_log_err_count(cdi, mmc, mrq);
}

static void mmc_sd_sec_clear_err_count(void)
{
	struct mmc_sd_sec_err_info *err_log = &sdi.err_info[0];
	struct mmc_sd_sec_status_err_info *status_err = &sdi.status_err;

	int i = 0;

	for (i = 0; i < MAX_LOG_INDEX; i++) {
		err_log[i].status = 0;
		err_log[i].first_issue_time = 0;
		err_log[i].last_issue_time = 0;
		err_log[i].count = 0;
	}

	memset(status_err, 0,
			sizeof(struct mmc_sd_sec_status_err_info));
}

static void mmc_sd_sec_init_err_count(struct mmc_sd_sec_err_info *err_log)
{
	static const char *const req_types[] = {
		"sbc  ", "cmd  ", "data ", "stop ", "busy "
	};
	int i;

	/*
	 * err_log[0].type = "sbc  "
	 * err_log[0].err_type = -EILSEQ;
	 * err_log[1].type = "sbc  "
	 * err_log[1].err_type = -ETIMEDOUT;
	 * ...
	 */
	for (i = 0; i < MAX_LOG_INDEX; i++) {
		snprintf(err_log[i].type, sizeof(char) * 5, "%s",
				req_types[i / MAX_ERR_TYPE_INDEX]);

		err_log[i].err_type =
			(i % MAX_ERR_TYPE_INDEX == 0) ?	-EILSEQ : -ETIMEDOUT;
	}
}

void sd_sec_card_event(struct mmc_host *mmc)
{
	bool status;

	status = mmc_gpio_get_cd(mmc) ? true : false;

	if (!status)
		mmc->unused = 0;

	mmc_sd_sec_clear_err_count();
}

void sd_sec_set_features(struct mmc_host *host)
{
	mmc_sd_sec_init_err_count(&sdi.err_info[0]);
	sd_sec_init_sysfs(host);
}

void mmc_sec_set_features(struct mmc_host *host)
{
	mmc_sd_sec_init_err_count(&mdi.err_info[0]);
	mmc_sec_init_sysfs(host);
}

MODULE_LICENSE("GPL v2");
