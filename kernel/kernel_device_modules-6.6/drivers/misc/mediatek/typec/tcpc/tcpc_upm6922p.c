// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/pm_runtime.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/cpu.h>
#include <linux/version.h>
#include <linux/sched/clock.h>

#include "inc/tcpci.h"
#include "inc/upm6922p.h"
#include "inc/tcpci_typec.h"

#if IS_ENABLED(CONFIG_RT_REGMAP)
#include "inc/rt-regmap.h"
#endif /* CONFIG_RT_REGMAP */
/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 start*/
#include <linux/power/gxy_psy_sysfs.h>

/**********************************************************
 *
 *   [extern for other module]
 *
 *********************************************************/
#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
extern void gxy_bat_set_tcpcinfo(enum gxy_bat_tcpc_info tinfo_data);
#endif
 /*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 end*/
#define UPM6922P_DRV_VERSION	"2.0.8_MTK"

struct upm6922p_chip {
	struct i2c_client *client;
	struct device *dev;
#if IS_ENABLED(CONFIG_RT_REGMAP)
	struct rt_regmap_device *m_dev;
#endif /* CONFIG_RT_REGMAP */
	struct tcpc_desc *tcpc_desc;
	struct tcpc_device *tcpc;

	int irq_gpio;
	int irq;
	int chip_id;
    int chip_func_sw;

	struct mutex irq_lock;
	bool is_suspended;
	bool irq_while_suspended;

	bool vconn_en;
	bool lpm_en;
};

static int upm6922p_set_bist_test_mode(struct tcpc_device *tcpc, bool en);
static int upm6922p_set_shutdown_power_mode(struct tcpc_device *tcpc, bool en);

#if IS_ENABLED(CONFIG_RT_REGMAP)
RT_REG_DECL(TCPC_V10_REG_VID, 2, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_PID, 2, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_DID, 2, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_TYPEC_REV, 2, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_PD_REV, 2, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_PDIF_REV, 2, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_ALERT, 2, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_ALERT_MASK, 4, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_TCPC_CTRL, 1, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_ROLE_CTRL, 1, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_FAULT_CTRL, 1, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_POWER_CTRL, 1, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_CC_STATUS, 1, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_POWER_STATUS, 1, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_FAULT_STATUS, 1, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_COMMAND, 1, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_MSG_HDR_INFO, 1, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_RX_DETECT, 1, RT_NORMAL_WR_ONCE, {});
RT_REG_DECL(TCPC_V10_REG_RX_BYTE_CNT, 32, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_TRANSMIT, 1, RT_VOLATILE, {});
RT_REG_DECL(TCPC_V10_REG_TX_BYTE_CNT, 31, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_VCONN_CTRL, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_CC_CTRL, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_VDR_DEF_STATUS, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_VDR_DEF_ALERT, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_VDR_DEF_ALERT_MASK, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_RESET_CTRL, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_HIDDEN_MODE, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_DEBUG_B9, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_TRIM_C4, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_TRIM_C5, 1, RT_VOLATILE, {});
RT_REG_DECL(UPM6922P_REG_TRIM_C6, 1, RT_VOLATILE, {});

static const rt_register_map_t upm6922p_chip_regmap[] = {
	RT_REG(TCPC_V10_REG_VID),
	RT_REG(TCPC_V10_REG_PID),
	RT_REG(TCPC_V10_REG_DID),
	RT_REG(TCPC_V10_REG_TYPEC_REV),
	RT_REG(TCPC_V10_REG_PD_REV),
	RT_REG(TCPC_V10_REG_PDIF_REV),
	RT_REG(TCPC_V10_REG_ALERT),
	RT_REG(TCPC_V10_REG_ALERT_MASK),
	RT_REG(TCPC_V10_REG_TCPC_CTRL),
	RT_REG(TCPC_V10_REG_ROLE_CTRL),
	RT_REG(TCPC_V10_REG_FAULT_CTRL),
	RT_REG(TCPC_V10_REG_POWER_CTRL),
	RT_REG(TCPC_V10_REG_CC_STATUS),
	RT_REG(TCPC_V10_REG_POWER_STATUS),
	RT_REG(TCPC_V10_REG_FAULT_STATUS),
	RT_REG(TCPC_V10_REG_COMMAND),
	RT_REG(TCPC_V10_REG_MSG_HDR_INFO),
	RT_REG(TCPC_V10_REG_RX_DETECT),
	RT_REG(TCPC_V10_REG_RX_BYTE_CNT),
	RT_REG(TCPC_V10_REG_TRANSMIT),
	RT_REG(TCPC_V10_REG_TX_BYTE_CNT),
	RT_REG(UPM6922P_REG_VCONN_CTRL),
	RT_REG(UPM6922P_REG_CC_CTRL),
	RT_REG(UPM6922P_REG_VDR_DEF_STATUS),
	RT_REG(UPM6922P_REG_VDR_DEF_ALERT),
	RT_REG(UPM6922P_REG_VDR_DEF_ALERT_MASK),
	RT_REG(UPM6922P_REG_RESET_CTRL),
	RT_REG(UPM6922P_REG_HIDDEN_MODE),
	RT_REG(UPM6922P_REG_DEBUG_B9),
	RT_REG(UPM6922P_REG_TRIM_C4),
	RT_REG(UPM6922P_REG_TRIM_C5),
	RT_REG(UPM6922P_REG_TRIM_C6),
};
#define UPM6922P_CHIP_REGMAP_SIZE ARRAY_SIZE(upm6922p_chip_regmap)

#endif /* CONFIG_RT_REGMAP */

static int upm6922p_read_device(void *client, u32 reg, int len, void *dst)
{
	struct i2c_client *i2c = client;
	int ret = 0, count = 5;
	u64 __maybe_unused t1 = 0, t2 = 0;

	while (1) {
		//t1 = local_clock();
		ret = i2c_smbus_read_i2c_block_data(i2c, reg, len, dst);
		//t2 = local_clock();
		//UPM6922P_INFO("%s del = %lluus, reg = 0x%02X, len = %d\n",
		//	    __func__, (t2 - t1) / NSEC_PER_USEC, reg, len);
		if (ret < 0 && count > 1)
			count--;
		else
			break;
		udelay(100);
	}
	return ret;
}

static int upm6922p_write_device(void *client, u32 reg, int len, const void *src)
{
	struct i2c_client *i2c = client;
	int ret = 0, count = 5;
	u64 __maybe_unused t1 = 0, t2 = 0;

	while (1) {
		//t1 = local_clock();
		ret = i2c_smbus_write_i2c_block_data(i2c, reg, len, src);
		//t2 = local_clock();
		//UPM6922P_INFO("%s del = %lluus, reg = 0x%02X, len = %d\n",
		//	    __func__, (t2 - t1) / NSEC_PER_USEC, reg, len);
		if (ret < 0 && count > 1)
			count--;
		else
			break;
		udelay(100);
	}
	return ret;
}

static int upm6922p_reg_read(struct i2c_client *i2c, u8 reg)
{
	struct upm6922p_chip *chip = i2c_get_clientdata(i2c);
	u8 val = 0;
	int ret = 0;

#if IS_ENABLED(CONFIG_RT_REGMAP)
	ret = rt_regmap_block_read(chip->m_dev, reg, 1, &val);
#else
	ret = upm6922p_read_device(chip->client, reg, 1, &val);
#endif /* CONFIG_RT_REGMAP */
	if (ret < 0) {
		dev_err(chip->dev, "upm6922p reg read fail\n");
		return ret;
	}
	return val;
}

static int upm6922p_reg_write(struct i2c_client *i2c, u8 reg, const u8 data)
{
	struct upm6922p_chip *chip = i2c_get_clientdata(i2c);
	int ret = 0;

#if IS_ENABLED(CONFIG_RT_REGMAP)
	ret = rt_regmap_block_write(chip->m_dev, reg, 1, &data);
#else
	ret = upm6922p_write_device(chip->client, reg, 1, &data);
#endif /* CONFIG_RT_REGMAP */
	if (ret < 0)
		dev_err(chip->dev, "upm6922p reg write fail\n");
	return ret;
}

static int upm6922p_block_read(struct i2c_client *i2c,
			u8 reg, int len, void *dst)
{
	struct upm6922p_chip *chip = i2c_get_clientdata(i2c);
	int ret = 0;
#if IS_ENABLED(CONFIG_RT_REGMAP)
	ret = rt_regmap_block_read(chip->m_dev, reg, len, dst);
#else
	ret = upm6922p_read_device(chip->client, reg, len, dst);
#endif /* #if IS_ENABLED(CONFIG_RT_REGMAP) */
	if (ret < 0)
		dev_err(chip->dev, "upm6922p block read fail\n");
	return ret;
}

static int upm6922p_block_write(struct i2c_client *i2c,
			u8 reg, int len, const void *src)
{
	struct upm6922p_chip *chip = i2c_get_clientdata(i2c);
	int ret = 0;
#if IS_ENABLED(CONFIG_RT_REGMAP)
	ret = rt_regmap_block_write(chip->m_dev, reg, len, src);
#else
	ret = upm6922p_write_device(chip->client, reg, len, src);
#endif /* #if IS_ENABLED(CONFIG_RT_REGMAP) */
	if (ret < 0)
		dev_err(chip->dev, "upm6922p block write fail\n");
	return ret;
}

static int32_t upm6922p_write_word(struct i2c_client *client,
					uint8_t reg_addr, uint16_t data)
{
	data = cpu_to_le16(data);
	return upm6922p_block_write(client, reg_addr, 2, &data);
}

static int32_t upm6922p_read_word(struct i2c_client *client,
					uint8_t reg_addr, uint16_t *data)
{
	int ret = upm6922p_block_read(client, reg_addr, 2, data);
	*data = le16_to_cpu(*data);
	return ret;
}

static inline int upm6922p_i2c_write8(
	struct tcpc_device *tcpc, u8 reg, const u8 data)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

	return upm6922p_reg_write(chip->client, reg, data);
}

static inline int upm6922p_i2c_write16(
		struct tcpc_device *tcpc, u8 reg, const u16 data)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

	return upm6922p_write_word(chip->client, reg, data);
}

static inline int upm6922p_i2c_read8(struct tcpc_device *tcpc, u8 reg)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

	return upm6922p_reg_read(chip->client, reg);
}

static inline int upm6922p_i2c_read16(
	struct tcpc_device *tcpc, u8 reg)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	u16 data;
	int ret;

	ret = upm6922p_read_word(chip->client, reg, &data);
	if (ret < 0)
		return ret;
	return data;
}

static int upm6922p_i2c_update_bits(struct tcpc_device *tcpc, u8 reg,
				  u8 val, u8 mask)
{
	u8 data = 0;
	int ret = 0;

	ret = upm6922p_i2c_read8(tcpc, reg);
	if (ret < 0)
		return ret;
	data = ret;

	data &= ~mask;
	data |= val & mask;

	return upm6922p_i2c_write8(tcpc, reg, data);
}

static inline int upm6922p_i2c_set_bits(struct tcpc_device *tcpc, u8 reg, u8 mask)
{
	return upm6922p_i2c_update_bits(tcpc, reg, mask, mask);
}

static inline int upm6922p_i2c_clr_bits(struct tcpc_device *tcpc, u8 reg, u8 mask)
{
	return upm6922p_i2c_update_bits(tcpc, reg, 0x00, mask);
}

#if IS_ENABLED(CONFIG_RT_REGMAP)
static struct rt_regmap_fops upm6922p_regmap_fops = {
	.read_device = upm6922p_read_device,
	.write_device = upm6922p_write_device,
};
#endif /* CONFIG_RT_REGMAP */

static int upm6922p_regmap_init(struct upm6922p_chip *chip)
{
#if IS_ENABLED(CONFIG_RT_REGMAP)
	struct rt_regmap_properties *props;
	char name[32];
	int len;

	props = devm_kzalloc(chip->dev, sizeof(*props), GFP_KERNEL);
	if (!props)
		return -ENOMEM;

	props->register_num = UPM6922P_CHIP_REGMAP_SIZE;
	props->rm = upm6922p_chip_regmap;

	props->rt_regmap_mode = RT_MULTI_BYTE |
				RT_IO_PASS_THROUGH | RT_DBG_SPECIAL;
	snprintf(name, sizeof(name), "upm6922p-%02x", chip->client->addr);

	len = strlen(name);
	props->name = kzalloc(len+1, GFP_KERNEL);
	props->aliases = kzalloc(len+1, GFP_KERNEL);

	if ((!props->name) || (!props->aliases))
		return -ENOMEM;

	strlcpy((char *)props->name, name, len+1);
	strlcpy((char *)props->aliases, name, len+1);
	props->io_log_en = 0;

	chip->m_dev = rt_regmap_device_register(props,
			&upm6922p_regmap_fops, chip->dev, chip->client, chip);
	if (!chip->m_dev) {
		dev_err(chip->dev, "upm6922p chip rt_regmap register fail\n");
		return -EINVAL;
	}
#endif
	return 0;
}

static int upm6922p_regmap_deinit(struct upm6922p_chip *chip)
{
#if IS_ENABLED(CONFIG_RT_REGMAP)
	rt_regmap_device_unregister(chip->m_dev);
#endif
	return 0;
}

static inline int upm6922p_software_reset(struct tcpc_device *tcpc)
{
#if IS_ENABLED(CONFIG_RT_REGMAP)
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
#endif /* CONFIG_RT_REGMAP */
	/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 start*/
	int ret = 0;

	ret = upm6922p_i2c_write8(tcpc, UPM6922P_REG_RESET_CTRL, UPM6922P_REG_SOFT_RESET);
	/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 end*/
	if (ret < 0)
		return ret;
#if IS_ENABLED(CONFIG_RT_REGMAP)
	rt_regmap_cache_reload(chip->m_dev);
#endif /* CONFIG_RT_REGMAP */
	usleep_range(1000, 2000);
	/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 start*/
	return upm6922p_i2c_write8(tcpc, UPM6922P_REG_RESET_CTRL,
            UPM6922P_REG_EXT_STATUS_MASK_CTRL |
            UPM6922P_REG_CABLE_RESET_CTRL);
	/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 end*/
}

static inline int upm6922p_command(struct tcpc_device *tcpc, uint8_t cmd)
{
	return upm6922p_i2c_write8(tcpc, TCPC_V10_REG_COMMAND, cmd);
}

static inline int upm6922p_init_alert_mask(struct tcpc_device *tcpc)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	uint16_t mask = TCPC_V10_REG_ALERT_CC_STATUS |
			TCPC_V10_REG_ALERT_POWER_STATUS |
			TCPC_V10_REG_ALERT_FAULT |
			/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 start*/
			// TCPC_V10_REG_EXTENDED_STATUS |
			/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 end*/
			TCPC_V10_REG_VBUS_SINK_DISCONNECT |
			TCPC_V10_REG_ALERT_VENDOR_DEFINED;
	uint8_t masks[4] = {0x00, 0x00,
			    TCPC_V10_REG_POWER_STATUS_VBUS_PRES,
			    TCPC_V10_REG_FAULT_STATUS_VCONN_OV |
			    TCPC_V10_REG_FAULT_STATUS_VCONN_OC};

#if IS_ENABLED(CONFIG_USB_POWER_DELIVERY)
	/* Need to handle RX overflow */
	mask |= TCPC_V10_REG_ALERT_TX_SUCCESS | TCPC_V10_REG_ALERT_TX_DISCARDED
			| TCPC_V10_REG_ALERT_TX_FAILED
			| TCPC_V10_REG_ALERT_RX_HARD_RST
			| TCPC_V10_REG_ALERT_RX_STATUS
			| TCPC_V10_REG_RX_OVERFLOW;
#endif
	*(uint16_t *)masks = cpu_to_le16(mask);
	return upm6922p_block_write(chip->client, TCPC_V10_REG_ALERT_MASK,
				  sizeof(masks), masks);
}

static int upm6922p_rx_alert_mask(struct tcpc_device *tcpc)
{
	int ret;
	uint16_t mask;
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

	ret = upm6922p_i2c_read16(tcpc, TCPC_V10_REG_ALERT_MASK);
	if (ret < 0)
		return ret;

	UPM6922P_INFO("rx_alert_mask:0x%x\n", ret);
	mask = (uint16_t) ret;

	mask &= ~TCPC_V10_REG_ALERT_RX_STATUS;
	mask &= ~TCPC_V10_REG_RX_OVERFLOW;

	return upm6922p_write_word(chip->client, TCPC_V10_REG_ALERT_MASK, mask);
}

static inline int upm6922p_init_up_mask(struct tcpc_device *tcpc)
{
	uint8_t rt_mask =   UPM6922P_REG_VSAFE0V_STATUS_MASK |
                        //UPM6922P_REG_RA_DETACH_MASK |
                        UPM6922P_REG_REF_DISCNT_MASK;

	return upm6922p_i2c_write8(tcpc, UPM6922P_REG_VDR_DEF_ALERT_MASK, rt_mask);
}

static irqreturn_t upm6922p_intr_handler(int irq, void *data)
{
	struct upm6922p_chip *chip = data;

	mutex_lock(&chip->irq_lock);
	if (chip->is_suspended) {
		dev_notice(chip->dev, "%s irq while suspended\n", __func__);
		chip->irq_while_suspended = true;
		disable_irq_nosync(chip->irq);
		mutex_unlock(&chip->irq_lock);
		return IRQ_NONE;
	}
	mutex_unlock(&chip->irq_lock);

	pm_stay_awake(chip->dev);
	tcpci_lock_typec(chip->tcpc);
	tcpci_alert(chip->tcpc, false);
	tcpci_unlock_typec(chip->tcpc);
	pm_relax(chip->dev);

	return IRQ_HANDLED;
}

static int upm6922p_init_alert(struct tcpc_device *tcpc)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	int ret = 0;
	char *name = NULL;

	/* Clear Alert Mask & Status */
	upm6922p_write_word(chip->client, TCPC_V10_REG_ALERT_MASK, 0);
	upm6922p_write_word(chip->client, TCPC_V10_REG_ALERT, 0xffff);

	name = devm_kasprintf(chip->dev, GFP_KERNEL, "%s-IRQ",
			      chip->tcpc_desc->name);
	if (!name)
		return -ENOMEM;

	dev_info(chip->dev, "%s name = %s, gpio = %d\n",
			    __func__, chip->tcpc_desc->name, chip->irq_gpio);

	ret = devm_gpio_request(chip->dev, chip->irq_gpio, name);
	if (ret < 0) {
		dev_notice(chip->dev, "%s request GPIO fail(%d)\n",
				      __func__, ret);
		return ret;
	}

	ret = gpio_direction_input(chip->irq_gpio);
	if (ret < 0) {
		dev_notice(chip->dev, "%s set GPIO fail(%d)\n", __func__, ret);
		return ret;
	}

	ret = gpio_to_irq(chip->irq_gpio);
	if (ret < 0) {
		dev_notice(chip->dev, "%s gpio to irq fail(%d)",
				      __func__, ret);
		return ret;
	}
	chip->irq = ret;

	dev_info(chip->dev, "%s IRQ number = %d\n", __func__, chip->irq);

	device_init_wakeup(chip->dev, true);
	ret = devm_request_threaded_irq(chip->dev, chip->irq, NULL,
					upm6922p_intr_handler,
					IRQF_TRIGGER_LOW | IRQF_ONESHOT,
					name, chip);
	if (ret < 0) {
		dev_notice(chip->dev, "%s request irq fail(%d)\n",
				      __func__, ret);
		return ret;
	}
	enable_irq_wake(chip->irq);

	return 0;
}

static int upm6922p_alert_status_clear(struct tcpc_device *tcpc, uint32_t mask)
{
	int ret;
	uint16_t mask_t1;
	uint8_t mask_t2;

	mask_t1 = mask;
	if (mask_t1) {
		ret = upm6922p_i2c_write16(tcpc, TCPC_V10_REG_ALERT, mask_t1);
		if (ret < 0)
			return ret;
	}

	mask_t2 = mask >> 16;
	if (mask_t2) {
		ret = upm6922p_i2c_write8(tcpc, UPM6922P_REG_VDR_DEF_ALERT, mask_t2);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int upm6922p_set_clock_gating(struct tcpc_device *tcpc, bool en)
{
	int ret = 0;
#if CONFIG_TCPC_CLOCK_GATING
	//struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	int i = 0;

	if (en) {
        UPM6922P_INFO("clear all alert\n");
		for (i = 0; i < 2; i++)
			ret = upm6922p_alert_status_clear(tcpc,
				TCPC_REG_ALERT_RX_ALL_MASK);
	}
#endif	/* CONFIG_TCPC_CLOCK_GATING */

	return ret;
}

static int upm6922p_idle_ctrl(struct tcpc_device *tcpc)
{
    //struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

    /* optional */

	return 0;
}

static int upm6922p_tcpc_init(struct tcpc_device *tcpc, bool sw_reset)
{
	int ret;
	//bool retry_discard_old = false;
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
    int data = 0;
	uint8_t tmp1 = 0;

	UPM6922P_INFO("\n");

	if (sw_reset) {
		ret = upm6922p_software_reset(tcpc);
		if (ret < 0)
			return ret;
	}

    upm6922p_set_shutdown_power_mode(tcpc, false);

	/* UFP Both RD setting */
	/* DRP = 0, RpVal = 0 (Default), Rd, Rd */
	upm6922p_i2c_write8(tcpc, TCPC_V10_REG_ROLE_CTRL,
		TCPC_V10_REG_ROLE_CTRL_RES_SET(0, 0, CC_RD, CC_RD));

	//upm6922p_i2c_write8(tcpc, TCPC_V10_REG_FAULT_CTRL,
	//	TCPC_V10_REG_FAULT_CTRL_DIS_VCONN_OV);
	upm6922p_command(tcpc, TCPM_CMD_ENABLE_VBUS_DETECT);

	/* RX/TX Clock Gating (Auto Mode)*/
	if (!sw_reset)
		upm6922p_set_clock_gating(tcpc, true);

	//if (!(tcpc->tcpc_flags & TCPC_FLAGS_RETRY_CRC_DISCARD))
	//	retry_discard_old = true;

	upm6922p_alert_status_clear(tcpc, 0xffffffff);

	upm6922p_init_up_mask(tcpc);
	upm6922p_init_alert_mask(tcpc);

	upm6922p_idle_ctrl(tcpc);

#if 1 // optimization at 1101
    chip->chip_func_sw = 0;

    upm6922p_i2c_write8(tcpc, UPM6922P_REG_HIDDEN_MODE, 0x6E);

    data = upm6922p_i2c_read8(tcpc, UPM6922P_REG_DEBUG_B9);
    if (data < 0) {
        pr_err("upm6922p: read 0xB9 fail, data:0x%x\n", data);
    }
    UPM6922P_INFO("upm6922p: default trim data [0xB9]:0x%x\n", data);

    tmp1 = (uint8_t) (data & UPM6922P_REG_FUNC_SW_MASK) >> UPM6922P_REG_FUNC_SW_SHIFT;

    if (tmp1 == UPM6922P_REG_FUNC_SW_ID) {
        chip->chip_func_sw =  1;
        upm6922p_i2c_write8(tcpc, UPM6922P_REG_HIDDEN_MODE, UPM6922P_REG_HMODE_EXIT);
        UPM6922P_INFO("upm6922p: exit hidden mode\n");
        return 0;
    }

    data = upm6922p_i2c_read8(tcpc, UPM6922P_REG_TRIM_C5);
    if (data < 0) {
        pr_err("upm6922p: read 0xC5 fail, data:0x%x\n", data);
    }
    UPM6922P_INFO("upm6922p: default trim data [0xC5]:0x%x\n", data);

    tmp1 = (uint8_t) data & UPM6922P_TRIM_R5_WIN_MASK;

    if (tmp1 != UPM6922P_TRIM_R5_WIN_ID) {
        tmp1 = UPM6922P_TRIM_R5_WIN_ID;
    }

    data = (data & (~UPM6922P_TRIM_R5_WIN_MASK)) | tmp1;
    UPM6922P_INFO("upm6922p: new trim data [0xC5]:0x%x\n", data);
    //data = 0x33;
    //UPM6922P_INFO("upm6922p: new trim data [0xC5]:0x%x\n", data);

    upm6922p_i2c_write8(tcpc, UPM6922P_REG_TRIM_C5, data);

    data = upm6922p_i2c_read8(tcpc, UPM6922P_REG_TRIM_C6);
    if (data < 0) {
        pr_err("upm6922p: read 0xC6 fail, data:0x%x\n", data);
    }
    UPM6922P_INFO("upm6922p: default trim data [0xC6]:0x%x\n", data);

    tmp1 = (uint8_t) data & UPM6922P_TRIM_C6_WIN_MASK;
    if (tmp1 != UPM6922P_TRIM_C6_WIN_ID)
        tmp1 = UPM6922P_TRIM_C6_WIN_ID;

    data =  (data & (~UPM6922P_TRIM_C6_WIN_MASK)) | tmp1;
    UPM6922P_INFO("upm6922p: new trim data [0xC6]:0x%x\n", data);

    upm6922p_i2c_write8(tcpc, UPM6922P_REG_TRIM_C6, data);

    upm6922p_i2c_write8(tcpc, UPM6922P_REG_HIDDEN_MODE, UPM6922P_REG_HMODE_EXIT);
#endif

	mdelay(1);

	return 0;
}

static inline int upm6922p_fault_status_vconn_ov(struct tcpc_device *tcpc)
{
	return upm6922p_i2c_clr_bits(tcpc, UPM6922P_REG_VCONN_CTRL,
				   UPM6922P_REG_VCONN_DISCHG_EN);
}

static int upm6922p_fault_status_clear(struct tcpc_device *tcpc, uint8_t status)
{
	if (status & TCPC_V10_REG_FAULT_STATUS_VCONN_OV)
		upm6922p_fault_status_vconn_ov(tcpc);

	return upm6922p_i2c_write8(tcpc, TCPC_V10_REG_FAULT_STATUS, status);
}

static int upm6922p_set_alert_mask(struct tcpc_device *tcpc, uint32_t mask)
{
	int ret = 0;

	ret = upm6922p_i2c_write16(tcpc, TCPC_V10_REG_ALERT_MASK, mask);
	if (ret < 0)
		return ret;

	return upm6922p_i2c_write8(tcpc, UPM6922P_REG_VDR_DEF_ALERT_MASK, mask >> 16);
}

static int upm6922p_get_alert_mask(struct tcpc_device *tcpc, uint32_t *mask)
{
	int ret;
	uint8_t v2;

	ret = upm6922p_i2c_read16(tcpc, TCPC_V10_REG_ALERT_MASK);
	if (ret < 0)
		return ret;

	*mask = (uint16_t) ret;

	ret = upm6922p_i2c_read8(tcpc, UPM6922P_REG_VDR_DEF_ALERT_MASK);
	if (ret < 0)
		return ret;

	v2 = (uint8_t) ret;
	*mask |= v2 << 16;

	return 0;
}

static int upm6922p_get_alert_status_and_mask(struct tcpc_device *tcpc,
					    uint32_t *alert, uint32_t *mask)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	int ret;
	uint8_t buf[4] = {0};

	ret = upm6922p_block_read(chip->client, TCPC_V10_REG_ALERT, 4, buf);
	if (ret < 0)
		return ret;
	*alert = le16_to_cpu(*(uint16_t *)&buf[0]);
	*mask = le16_to_cpu(*(uint16_t *)&buf[2]);

	ret = upm6922p_block_read(chip->client, UPM6922P_REG_VDR_DEF_ALERT, 2, buf);
	if (ret < 0)
		return ret;
	*alert |= buf[0] << 16;
	*mask |= buf[1] << 16;

	return 0;
}

static int upm6922p_get_power_status(struct tcpc_device *tcpc)
{
	int ret;

	ret = upm6922p_i2c_read8(tcpc, TCPC_V10_REG_POWER_STATUS);
	if (ret < 0)
		return ret;
	tcpc->vbus_present = !!(ret & TCPC_V10_REG_POWER_STATUS_VBUS_PRES);

	ret = upm6922p_i2c_read8(tcpc, UPM6922P_REG_VDR_DEF_STATUS);
	if (ret < 0)
		return ret;
	tcpc->vbus_safe0v = !!(ret & UPM6922P_REG_VSAFE0V_STATUS);
	return 0;
}

static int upm6922p_get_fault_status(struct tcpc_device *tcpc, uint8_t *status)
{
	int ret;

	ret = upm6922p_i2c_read8(tcpc, TCPC_V10_REG_FAULT_STATUS);
	if (ret < 0)
		return ret;
	*status = (uint8_t) ret;
	return 0;
}

static int upm6922p_get_cc(struct tcpc_device *tcpc, int *cc1, int *cc2)
{
	int status, role_ctrl, cc_role;
	bool act_as_sink, act_as_drp;

	status = upm6922p_i2c_read8(tcpc, TCPC_V10_REG_CC_STATUS);
	if (status < 0)
		return status;

#if 0
	if (status & TCPC_V10_REG_CC_STATUS_DRP_TOGGLING) {
		*cc1 = TYPEC_CC_DRP_TOGGLING;
		*cc2 = TYPEC_CC_DRP_TOGGLING;
		return 0;
	}
#endif

	*cc1 = TCPC_V10_REG_CC_STATUS_CC1(status);
	*cc2 = TCPC_V10_REG_CC_STATUS_CC2(status);

	role_ctrl = upm6922p_i2c_read8(tcpc, TCPC_V10_REG_ROLE_CTRL);
	if (role_ctrl < 0)
		return role_ctrl;

	act_as_drp = TCPC_V10_REG_ROLE_CTRL_DRP & role_ctrl;

	if (act_as_drp) {
		act_as_sink = TCPC_V10_REG_CC_STATUS_DRP_RESULT(status);
	} else {
		if (tcpc->typec_polarity)
			cc_role = TCPC_V10_REG_CC_STATUS_CC2(role_ctrl);
		else
			cc_role = TCPC_V10_REG_CC_STATUS_CC1(role_ctrl);
		if (cc_role == TYPEC_CC_RP)
			act_as_sink = false;
		else
			act_as_sink = true;
	}

	/*
	 * If status is not open, then OR in termination to convert to
	 * enum tcpc_cc_voltage_status.
	 */
	if (*cc1 != TYPEC_CC_VOLT_OPEN)
		*cc1 |= (act_as_sink << 2);

	if (*cc2 != TYPEC_CC_VOLT_OPEN)
		*cc2 |= (act_as_sink << 2);

	return 0;
}

static int upm6922p_enable_vsafe0v_detect(
	struct tcpc_device *tcpc, bool enable)
{
	return (enable ? upm6922p_i2c_set_bits : upm6922p_i2c_clr_bits)
		(tcpc, UPM6922P_REG_VDR_DEF_ALERT_MASK, UPM6922P_REG_VSAFE0V_STATUS_MASK);
}

static int upm6922p_set_cc(struct tcpc_device *tcpc, int pull)
{
	int ret;
	uint8_t data;
	int rp_lvl = TYPEC_CC_PULL_GET_RP_LVL(pull), pull1, pull2;

	UPM6922P_INFO("pull = 0x%02X\n", pull);
	pull = TYPEC_CC_PULL_GET_RES(pull);
	if (pull == TYPEC_CC_DRP) {
		data = TCPC_V10_REG_ROLE_CTRL_RES_SET(
				1, rp_lvl, TYPEC_CC_RD, TYPEC_CC_RD);

		ret = upm6922p_i2c_write8(
			tcpc, TCPC_V10_REG_ROLE_CTRL, data);

		if (ret == 0) {
			upm6922p_enable_vsafe0v_detect(tcpc, false);
			ret = upm6922p_command(tcpc, TCPM_CMD_LOOK_CONNECTION);
		}
	} else {
		pull1 = pull2 = pull;

		if (pull == TYPEC_CC_RP &&
		    tcpc->typec_state == typec_attached_src) {
			if (tcpc->typec_polarity)
				pull1 = TYPEC_CC_OPEN;
			else
				pull2 = TYPEC_CC_OPEN;
		}
		data = TCPC_V10_REG_ROLE_CTRL_RES_SET(0, rp_lvl, pull1, pull2);
		ret = upm6922p_i2c_write8(tcpc, TCPC_V10_REG_ROLE_CTRL, data);
	}

	return 0;
}

static int upm6922p_set_polarity(struct tcpc_device *tcpc, int polarity)
{
	//int data;

	return (polarity ? upm6922p_i2c_set_bits : upm6922p_i2c_clr_bits)
		(tcpc, TCPC_V10_REG_TCPC_CTRL,
		 TCPC_V10_REG_TCPC_CTRL_PLUG_ORIENT);
}

static int upm6922p_set_vconn(struct tcpc_device *tcpc, int enable)
{
	int rv;
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

	chip->vconn_en = !!enable;
	rv = upm6922p_idle_ctrl(tcpc);
	if (rv < 0)
		return rv;

	return (enable ? upm6922p_i2c_set_bits : upm6922p_i2c_clr_bits)
		(tcpc, TCPC_V10_REG_POWER_CTRL, TCPC_V10_REG_POWER_CTRL_VCONN);
}

static int upm6922p_set_low_power_mode(
		struct tcpc_device *tcpc, bool en, int pull)
{
	int ret = 0;
	//uint8_t data;
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);

	chip->lpm_en = !!en;
	ret = upm6922p_idle_ctrl(tcpc);
	if (ret < 0)
		return ret;
	return upm6922p_enable_vsafe0v_detect(tcpc, !en);
}

static int upm6922p_set_shutdown_power_mode(struct tcpc_device *tcpc, bool en)
{
	int data = 0;

	data = upm6922p_i2c_read8(tcpc, UPM6922P_REG_CC_CTRL);
	if (data < 0) {
		pr_err("%s: read CC_CTRL fail, data:%d\n", __func__, data);
		return data;
	}

	UPM6922P_INFO("upm6922p: cc_ctrl:0x%x\n", data);

	if (en) {
		data |= UPM6922P_REG_DISABLED_REQ;
		UPM6922P_INFO("upm6922p: enter shutdown mode, cc_ctrl:0x%x\n", data);
	} else {
		data &= ~UPM6922P_REG_DISABLED_REQ;
		UPM6922P_INFO("upm6922p: exit shutdown mode, cc_ctrl:0x%x\n", data);
	}

	return upm6922p_i2c_write8(tcpc, UPM6922P_REG_CC_CTRL, data);
}

static int upm6922p_tcpc_deinit(struct tcpc_device *tcpc)
{
#if IS_ENABLED(CONFIG_RT_REGMAP)
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
#endif /* CONFIG_RT_REGMAP */

#if CONFIG_TCPC_SHUTDOWN_CC_DETACH
    /*A06_V code for AL7160AV-495 by jiashixian at 20250325 start*/
    upm6922p_set_cc(tcpc, TYPEC_CC_OPEN);
    mdelay(100);
    upm6922p_set_cc(tcpc, TYPEC_CC_RD);
    mdelay(10);
    /*A06_V code for AL7160AV-495 by jiashixian at 20250325 end*/
#else
	upm6922p_i2c_write8(tcpc, UPM6922P_REG_RESET_CTRL, 1);
#endif	/* CONFIG_TCPC_SHUTDOWN_CC_DETACH */
#if IS_ENABLED(CONFIG_RT_REGMAP)
	rt_regmap_cache_reload(chip->m_dev);
#endif /* CONFIG_RT_REGMAP */
    upm6922p_set_shutdown_power_mode(tcpc, true);

	return 0;
}

#if IS_ENABLED(CONFIG_USB_POWER_DELIVERY)
static int upm6922p_set_msg_header(
	struct tcpc_device *tcpc, uint8_t power_role, uint8_t data_role)
{
	uint8_t msg_hdr = TCPC_V10_REG_MSG_HDR_INFO_SET(
		data_role, power_role);

	return upm6922p_i2c_write8(
		tcpc, TCPC_V10_REG_MSG_HDR_INFO, msg_hdr);
}

static int upm6922p_set_rx_enable(struct tcpc_device *tcpc, uint8_t enable)
{
	int ret = 0;

	if (enable)
		ret = upm6922p_set_clock_gating(tcpc, false);

	if (ret == 0)
		ret = upm6922p_i2c_write8(tcpc, TCPC_V10_REG_RX_DETECT, enable);

	if ((ret == 0) && (!enable)) {
		ret = upm6922p_set_clock_gating(tcpc, true);
	}

	return ret;
}

static int upm6922p_get_message(struct tcpc_device *tcpc, uint32_t *payload,
			uint16_t *msg_head, enum tcpm_transmit_type *frame_type)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	int rv = 0;
	uint8_t cnt = 0, buf[32];
	/*A06_V code for AL7160AV-495 by jiashixian at 20250325 start*/
	//const uint16_t alert_rx =
	//TCPC_V10_REG_ALERT_RX_STATUS|TCPC_V10_REG_RX_OVERFLOW;
	/*A06_V code for AL7160AV-495 by jiashixian at 20250325 end*/
	rv = upm6922p_block_read(chip->client, TCPC_V10_REG_RX_BYTE_CNT,
			       sizeof(buf), buf);
	if (rv < 0)
		return rv;

	cnt = buf[0];
	*frame_type = buf[1];
	*msg_head = le16_to_cpu(*(uint16_t *)&buf[2]);

    if (*msg_head == 0x77a3)
		upm6922p_set_bist_test_mode(tcpc, true);

	/* TCPC 1.0 ==> no need to subtract the size of msg_head */
	if (cnt > 3) {
		cnt -= 3; /* MSG_HDR */
		if (cnt > sizeof(buf) - 4)
			cnt = sizeof(buf) - 4;
		memcpy(payload, buf + 4, cnt);
	}

    /* Read complete, clear RX status alert bit */
    /*A06_V code for AL7160AV-495 by jiashixian at 20250325 start*/
    //if (*msg_head != 0x77a3)
    //	upm6922p_alert_status_clear(tcpc, alert_rx);
    /*A06_V code for AL7160AV-495 by jiashixian at 20250325 end*/
	return rv;
}

static int upm6922p_set_bist_carrier_mode(
	struct tcpc_device *tcpc, uint8_t pattern)
{
	/* Don't support this function */
	return 0;
}

#if CONFIG_USB_PD_RETRY_CRC_DISCARD
static int upm6922p_retransmit(struct tcpc_device *tcpc)
{
	return upm6922p_i2c_write8(tcpc, TCPC_V10_REG_TRANSMIT,
			TCPC_V10_REG_TRANSMIT_SET(
			tcpc->pd_retry_count, TCPC_TX_SOP));
}
#endif	/* CONFIG_USB_PD_RETRY_CRC_DISCARD */

#pragma pack(push, 1)
struct tcpc_transmit_packet {
	uint8_t cnt;
	uint16_t msg_header;
	uint8_t data[sizeof(uint32_t)*7];
};
#pragma pack(pop)

static int upm6922p_transmit(struct tcpc_device *tcpc,
	enum tcpm_transmit_type type, uint16_t header, const uint32_t *data)
{
	struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
	int rv;
	int data_cnt;
	struct tcpc_transmit_packet packet;

	if (type < TCPC_TX_HARD_RESET) {
		data_cnt = sizeof(uint32_t) * PD_HEADER_CNT(header);

		packet.cnt = data_cnt + sizeof(uint16_t);
		packet.msg_header = header;

		if (data_cnt > 0)
			memcpy(packet.data, (uint8_t *) data, data_cnt);

		rv = upm6922p_block_write(chip->client,
				TCPC_V10_REG_TX_BYTE_CNT,
				packet.cnt+1, (uint8_t *) &packet);
		if (rv < 0)
			return rv;
	}

	rv = upm6922p_i2c_write8(tcpc, TCPC_V10_REG_TRANSMIT,
			TCPC_V10_REG_TRANSMIT_SET(
			tcpc->pd_retry_count, type));
	return rv;
}

static int upm6922p_set_bist_test_mode(struct tcpc_device *tcpc, bool en)
{
    struct upm6922p_chip *chip = tcpc_get_dev_data(tcpc);
    //int data;

    UPM6922P_INFO("en:%d", en);

	if (en) {
		upm6922p_rx_alert_mask(tcpc);
	} else {
		if (chip->chip_func_sw == 0) {
			upm6922p_tcpc_init(tcpc, true);
			upm6922p_set_cc(tcpc,TYPEC_CC_DRP);
		}
		upm6922p_i2c_write16(tcpc, TCPC_V10_REG_ALERT, 0x0404);
		upm6922p_i2c_write16(tcpc, TCPC_V10_REG_ALERT, 0x0404);
		upm6922p_init_alert_mask(tcpc);
	}

	return (en ? upm6922p_i2c_set_bits : upm6922p_i2c_clr_bits)
		(tcpc, TCPC_V10_REG_TCPC_CTRL,
		 TCPC_V10_REG_TCPC_CTRL_BIST_TEST_MODE);
}
#endif /* CONFIG_USB_POWER_DELIVERY */

static struct tcpc_ops upm6922p_tcpc_ops = {
	.init = upm6922p_tcpc_init,
	.alert_status_clear = upm6922p_alert_status_clear,
	.fault_status_clear = upm6922p_fault_status_clear,
	.set_alert_mask = upm6922p_set_alert_mask,
	.get_alert_mask = upm6922p_get_alert_mask,
	.get_alert_status_and_mask = upm6922p_get_alert_status_and_mask,
	.get_power_status = upm6922p_get_power_status,
	.get_fault_status = upm6922p_get_fault_status,
	.get_cc = upm6922p_get_cc,
	.set_cc = upm6922p_set_cc,
	.set_polarity = upm6922p_set_polarity,
	.set_vconn = upm6922p_set_vconn,
	.deinit = upm6922p_tcpc_deinit,

	.set_low_power_mode = upm6922p_set_low_power_mode,

#if IS_ENABLED(CONFIG_USB_POWER_DELIVERY)
	.set_msg_header = upm6922p_set_msg_header,
	.set_rx_enable = upm6922p_set_rx_enable,
	.protocol_reset = NULL,
	.get_message = upm6922p_get_message,
	.transmit = upm6922p_transmit,
	.set_bist_test_mode = upm6922p_set_bist_test_mode,
	.set_bist_carrier_mode = upm6922p_set_bist_carrier_mode,
#endif	/* CONFIG_USB_POWER_DELIVERY */

#if CONFIG_USB_PD_RETRY_CRC_DISCARD
	.retransmit = upm6922p_retransmit,
#endif	/* CONFIG_USB_PD_RETRY_CRC_DISCARD */
};

static int up_parse_dt(struct upm6922p_chip *chip, struct device *dev)
{
	struct device_node *np = dev->of_node;
	int ret = 0;

	pr_info("%s\n", __func__);

#if !IS_ENABLED(CONFIG_MTK_GPIO) || IS_ENABLED(CONFIG_MTK_GPIOLIB_STAND)
	ret = of_get_named_gpio(np, "upm6922ppd,intr-gpio", 0);
	if (ret < 0)
		ret = of_get_named_gpio(np, "upm6922ppd,intr_gpio", 0);

	if (ret < 0)
		pr_err("%s no intr_gpio info\n", __func__);
	else
		chip->irq_gpio = ret;
#else
	ret = of_property_read_u32(np, "upm6922ppd,intr-gpio-num", &chip->irq_gpio) ?
	      of_property_read_u32(np, "upm6922ppd,intr_gpio_num", &chip->irq_gpio) : 0;
	if (ret < 0)
		pr_err("%s no intr_gpio info\n", __func__);
#endif /* !CONFIG_MTK_GPIO || CONFIG_MTK_GPIOLIB_STAND */
	return ret < 0 ? ret : 0;
}

static int upm6922p_tcpcdev_init(struct upm6922p_chip *chip, struct device *dev)
{
	struct tcpc_desc *desc;
	struct tcpc_device *tcpc = NULL;
	struct device_node *np = dev->of_node;
	u32 val, len;
	const char *name = "default";

	dev_info(dev, "%s\n", __func__);

	desc = devm_kzalloc(dev, sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;
	if (of_property_read_u32(np, "up-tcpc,role-def", &val) >= 0 ||
	    of_property_read_u32(np, "up-tcpc,role_def", &val) >= 0) {
		if (val >= TYPEC_ROLE_NR)
			desc->role_def = TYPEC_ROLE_DRP;
		else
			desc->role_def = val;
	} else {
		dev_info(dev, "use default Role DRP\n");
		desc->role_def = TYPEC_ROLE_DRP;
	}

	if (of_property_read_u32(np, "up-tcpc,rp-level", &val) >= 0 ||
	    of_property_read_u32(np, "up-tcpc,rp_level", &val) >= 0) {
		switch (val) {
		case TYPEC_RP_DFT:
		case TYPEC_RP_1_5:
		case TYPEC_RP_3_0:
			desc->rp_lvl = val;
			break;
		default:
			desc->rp_lvl = TYPEC_RP_DFT;
			break;
		}
	}

	if (of_property_read_u32(np, "up-tcpc,vconn-supply", &val) >= 0 ||
	    of_property_read_u32(np, "up-tcpc,vconn_supply", &val) >= 0) {
		if (val >= TCPC_VCONN_SUPPLY_NR)
			desc->vconn_supply = TCPC_VCONN_SUPPLY_ALWAYS;
		else
			desc->vconn_supply = val;
	} else {
		dev_info(dev, "use default VconnSupply\n");
		desc->vconn_supply = TCPC_VCONN_SUPPLY_ALWAYS;
	}

	if (of_property_read_string(np, "up-tcpc,name",
				(char const **)&name) < 0) {
		dev_info(dev, "use default name\n");
	}

	len = strlen(name);
	desc->name = kzalloc(len+1, GFP_KERNEL);
	if (!desc->name)
		return -ENOMEM;

	strlcpy((char *)desc->name, name, len+1);

	chip->tcpc_desc = desc;

	tcpc = tcpc_device_register(dev, desc, &upm6922p_tcpc_ops, chip);
	if (IS_ERR_OR_NULL(tcpc))
		return -EINVAL;
	chip->tcpc = tcpc;

#if CONFIG_USB_PD_DISABLE_PE
	tcpc->disable_pe = of_property_read_bool(np, "up-tcpc,disable-pe") ||
				 of_property_read_bool(np, "up-tcpc,disable_pe");
#endif	/* CONFIG_USB_PD_DISABLE_PE */

	tcpc->tcpc_flags = TCPC_FLAGS_VCONN_SAFE5V_ONLY;

#if CONFIG_USB_PD_RETRY_CRC_DISCARD
	tcpc->tcpc_flags |= TCPC_FLAGS_RETRY_CRC_DISCARD;
#endif  /* CONFIG_USB_PD_RETRY_CRC_DISCARD */

#if CONFIG_USB_PD_REV30
	tcpc->tcpc_flags |= TCPC_FLAGS_PD_REV30;

	if (tcpc->tcpc_flags & TCPC_FLAGS_PD_REV30)
		dev_info(dev, "PD_REV30\n");
	else
		dev_info(dev, "PD_REV20\n");
#endif	/* CONFIG_USB_PD_REV30 */
	tcpc->tcpc_flags |= TCPC_FLAGS_ALERT_V10;

	return 0;
}

#define UNISEMI_7610_VID	0x362f
#define UNISEMI_7610_PID	0x7610
/*A06_V code for SR-AL7160V-01-133 by yexuedong at 20240829 start*/
#define UPM6922P_CHG_ADDR 0x6B
#define UPM7610_ADDR 0x60
#define UPM7610_DEBUG_MASK 0x6E
#define UPM6922P_CHG_DEBUG_ADDR 0xD2
#define UPM6922P_DEBUG_MODE_ADDR 0XA9
#define UPM6922P_CHG_MASK 0x40
/*A06_V code for SR-AL7160V-01-133 by yexuedong at 20240829 end*/
static inline int upm6922p_check_revision(struct i2c_client *client)
{
	u16 vid, pid, did;
	int ret;
	u8 data = 0;
    u8 chg_data = 0;
    /*A06_V code for SR-AL7160V-01-133 by yexuedong at 20240829 start*/
    u8 val=0;
    chg_data = UPM7610_DEBUG_MASK;
    client->addr = UPM6922P_CHG_ADDR;
    ret = upm6922p_write_device(client, UPM6922P_DEBUG_MODE_ADDR, 1, &chg_data);
    if (ret < 0) {
        pr_err("upm6922 write 0XA9 fail\n");
        return -ENODEV;
    }
    ret = upm6922p_read_device(client, UPM6922P_CHG_DEBUG_ADDR, 1, &val);
    if (ret < 0) {
        pr_err("upm6922 block read fail\n");
    }
    pr_err("upm6922 read Reg[0xD2] = 0x%.2x\n", val);
    val = !!(val & UPM6922P_CHG_MASK);
    if (val) {
        pr_err("It is not upm6922P return !\n");
        return -EIO;
    }
    chg_data = 0;
    ret = upm6922p_write_device(client, UPM6922P_DEBUG_MODE_ADDR, 1, &chg_data);
    if (ret < 0) {
        pr_err("upm6922 reset 0XA9 fail\n");
        return -ENODEV;
    }
    client->addr = UPM7610_ADDR;
    /*A06_V code for SR-AL7160V-01-133 by yexuedong at 20240829 start*/
	ret = upm6922p_read_device(client, TCPC_V10_REG_VID, 2, &vid);
	if (ret < 0) {
		dev_notice(&client->dev, "read chip ID fail(%d)\n", ret);
		return -EIO;
	}

	if (vid != UNISEMI_7610_VID) {
		pr_info("%s failed, VID=0x%04x\n", __func__, vid);
		return -ENODEV;
	}

	ret = upm6922p_read_device(client, TCPC_V10_REG_PID, 2, &pid);
	if (ret < 0) {
		dev_notice(&client->dev, "read product ID fail(%d)\n", ret);
		return -EIO;
	}

	if (pid != UNISEMI_7610_PID) {
		pr_info("%s failed, PID=0x%04x\n", __func__, pid);
		return -ENODEV;
	}
	
	/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 start*/
	data = UPM6922P_REG_SOFT_RESET;
	ret = upm6922p_write_device(client, UPM6922P_REG_RESET_CTRL, 1, &data);
	if (ret < 0)
		return ret;

	usleep_range(1000, 2000);

	data = UPM6922P_REG_EXT_STATUS_MASK_CTRL | UPM6922P_REG_CABLE_RESET_CTRL;
	ret = upm6922p_write_device(client, UPM6922P_REG_RESET_CTRL, 1, &data);
	if (ret < 0)
		return ret;
	/*A06_V code for SR-AL7160V-01-135 by yexuedong at 20240828 end*/

	ret = upm6922p_read_device(client, TCPC_V10_REG_DID, 2, &did);
	if (ret < 0) {
		dev_notice(&client->dev, "read device ID fail(%d)\n", ret);
		return -EIO;
	}

	return did;
}

static int upm6922p_i2c_probe(struct i2c_client *client)
{
	struct upm6922p_chip *chip;
	int ret = 0, chip_id;
	bool use_dt = client->dev.of_node;

	pr_info("%s (%s)\n", __func__, UPM6922P_DRV_VERSION);
	if (i2c_check_functionality(client->adapter,
			I2C_FUNC_SMBUS_I2C_BLOCK | I2C_FUNC_SMBUS_BYTE_DATA))
		pr_info("I2C functionality : OK...\n");
	else
		pr_info("I2C functionality check : failuare...\n");

	chip_id = upm6922p_check_revision(client);
	if (chip_id < 0)
		return chip_id;
	/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 start*/
	/* set for tcpc_info */
	#if IS_ENABLED(CONFIG_HQ_PROJECT_O8)
	gxy_bat_set_tcpcinfo(GXY_BAT_TCPC_INFO_UPM6922P);
	#endif
	/*A06_V code for SR-AL7160V-01-97 by yexuedong at 20240902 end*/
	chip = devm_kzalloc(&client->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	if (use_dt) {
		ret = up_parse_dt(chip, &client->dev);
		if (ret < 0)
			return ret;
	} else {
		dev_err(&client->dev, "no dts node\n");
		return -ENODEV;
	}
	chip->dev = &client->dev;
	chip->client = client;
	i2c_set_clientdata(client, chip);
	chip->chip_id = chip_id;
	pr_info("upm6922p_chipID = 0x%0x\n", chip_id);
	mutex_init(&chip->irq_lock);
	chip->is_suspended = false;
	chip->irq_while_suspended = false;
	chip->vconn_en = false;
	chip->lpm_en = false;

	ret = upm6922p_regmap_init(chip);
	if (ret < 0) {
		dev_err(chip->dev, "upm6922p regmap init fail\n");
		goto err_regmap_init;
	}

	ret = upm6922p_tcpcdev_init(chip, &client->dev);
	if (ret < 0) {
		dev_err(&client->dev, "upm6922p tcpc dev init fail\n");
		goto err_tcpc_reg;
	}

	ret = upm6922p_init_alert(chip->tcpc);
	if (ret < 0) {
		pr_err("upm6922p init alert fail\n");
		goto err_irq_init;
	}

	pr_info("%s probe OK!\n", __func__);
	return 0;

err_irq_init:
	tcpc_device_unregister(chip->dev, chip->tcpc);
err_tcpc_reg:
	upm6922p_regmap_deinit(chip);
err_regmap_init:
	mutex_destroy(&chip->irq_lock);
	return ret;
}

static void upm6922p_i2c_remove(struct i2c_client *client)
{
	struct upm6922p_chip *chip = i2c_get_clientdata(client);

	if (chip) {
		tcpc_device_unregister(chip->dev, chip->tcpc);
		upm6922p_regmap_deinit(chip);
		mutex_destroy(&chip->irq_lock);
	}
}

#if CONFIG_PM
static int upm6922p_i2c_suspend(struct device *dev)
{
	struct upm6922p_chip *chip = dev_get_drvdata(dev);

	dev_info(dev, "%s irq_gpio = %d\n",
		      __func__, gpio_get_value(chip->irq_gpio));

	mutex_lock(&chip->irq_lock);
	chip->is_suspended = true;
	mutex_unlock(&chip->irq_lock);

	synchronize_irq(chip->irq);

	return tcpm_suspend(chip->tcpc);
}

static int upm6922p_i2c_resume(struct device *dev)
{
	struct upm6922p_chip *chip = dev_get_drvdata(dev);

	dev_info(dev, "%s irq_gpio = %d\n",
		      __func__, gpio_get_value(chip->irq_gpio));

	tcpm_resume(chip->tcpc);

	mutex_lock(&chip->irq_lock);
	if (chip->irq_while_suspended) {
		enable_irq(chip->irq);
		chip->irq_while_suspended = false;
	}
	chip->is_suspended = false;
	mutex_unlock(&chip->irq_lock);

	return 0;
}

static void upm6922p_shutdown(struct i2c_client *client)
{
	struct upm6922p_chip *chip = i2c_get_clientdata(client);

	/* Please reset IC here */
	if (chip != NULL) {
		if (chip->irq)
			disable_irq(chip->irq);
		tcpm_shutdown(chip->tcpc);
	} else {
		i2c_smbus_write_byte_data(
			client, UPM6922P_REG_RESET_CTRL, 0x01);
        i2c_smbus_write_byte_data(
			client, UPM6922P_REG_CC_CTRL, 0x44);
	}
}

#if IS_ENABLED(CONFIG_PM_RUNTIME)
static int upm6922p_pm_suspend_runtime(struct device *device)
{
	dev_dbg(device, "pm_runtime: suspending...\n");
	return 0;
}

static int upm6922p_pm_resume_runtime(struct device *device)
{
	dev_dbg(device, "pm_runtime: resuming...\n");
	return 0;
}
#endif /* CONFIG_PM_RUNTIME */

static const struct dev_pm_ops upm6922p_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(
			upm6922p_i2c_suspend,
			upm6922p_i2c_resume)
#if IS_ENABLED(CONFIG_PM_RUNTIME)
	SET_RUNTIME_PM_OPS(
		upm6922p_pm_suspend_runtime,
		upm6922p_pm_resume_runtime,
		NULL
	)
#endif /* CONFIG_PM_RUNTIME */
};
#define UPM6922P_PM_OPS	(&upm6922p_pm_ops)
#else
#define UPM6922P_PM_OPS	(NULL)
#endif /* CONFIG_PM */

static const struct i2c_device_id upm6922p_id_table[] = {
	{"upm6922p", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, upm6922p_id_table);

static const struct of_device_id up_match_table[] = {
	{.compatible = "unisemipower,upm6922p",},
	{},
};

static struct i2c_driver upm6922p_driver = {
	.driver = {
		.name = "upm6922p",
		.owner = THIS_MODULE,
		.of_match_table = up_match_table,
		.pm = UPM6922P_PM_OPS,
	},
	.probe = upm6922p_i2c_probe,
	.remove = upm6922p_i2c_remove,
	.shutdown = upm6922p_shutdown,
	.id_table = upm6922p_id_table,
};

static int __init upm6922p_init(void)
{
	struct device_node *np;

	pr_info("%s (%s)\n", __func__, UPM6922P_DRV_VERSION);
	np = of_find_node_by_name(NULL, "upm6922p");
	pr_info("%s upm6922p node %s\n", __func__,
		np == NULL ? "not found" : "found");

	return i2c_add_driver(&upm6922p_driver);
}
subsys_initcall(upm6922p_init);

static void __exit upm6922p_exit(void)
{
	i2c_del_driver(&upm6922p_driver);
}
module_exit(upm6922p_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DuLai <lai.du@unisemipower.com>");
MODULE_DESCRIPTION("UPM6922P TCPC Driver");
MODULE_VERSION(UPM6922P_DRV_VERSION);

