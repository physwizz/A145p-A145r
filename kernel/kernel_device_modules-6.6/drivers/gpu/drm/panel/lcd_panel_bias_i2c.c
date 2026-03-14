// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#include <linux/ratelimit.h>
#include "lcd_panel_bias_i2c.h"

/* i2c control start */
#define LCD_PANEL_I2C_ID_NAME "I2C_LCD_BIAS"
static struct i2c_client *lcd_panel_i2c_client;

struct lcd_panel_i2c_dev {
	struct i2c_client *client;
};

int lcd_panel_i2c_write_bytes(unsigned char addr, unsigned char value)
{
	static DEFINE_RATELIMIT_STATE(write_ratelimit, 1 * HZ, 1);
	struct i2c_client *client = lcd_panel_i2c_client;
	char write_data[2] = { 0 };
	int ret = 0;

	if (client == NULL) {
		pr_err("%s: ERROR!! client is null\n", __func__);
		return 0;
	}

	pr_debug("%s: name=%s addr=0x%x, write:0x%x, value:0x%x\n", __func__, client->name,
		 client->addr, addr, value);

	write_data[0] = addr;
	write_data[1] = value;
	ret = i2c_master_send(client, write_data, 2);
	if (ret < 0) {
		if (__ratelimit(&write_ratelimit))
			pr_err("%s: ERROR write 0x%x with 0x%x fail %d\n",
			__func__, addr, value, ret);
	}

	return ret;
}
EXPORT_SYMBOL(lcd_panel_i2c_write_bytes);

int lcd_panel_i2c_read_bytes(unsigned char addr, unsigned char *returnData)
{
	char cmd_buf[2] = { 0x00, 0x00 };
	static DEFINE_RATELIMIT_STATE(read_ratelimit, 1 * HZ, 1);
	struct i2c_client *client = lcd_panel_i2c_client;
	int ret = 0;

	if (client == NULL) {
		pr_err("%s: ERROR!! lcd_panel_i2c_client is null\n", __func__);
		return 0;
	}

	cmd_buf[0] = addr;
	ret = i2c_master_send(client, &cmd_buf[0], 1);
	ret = i2c_master_recv(client, &cmd_buf[1], 1);
	if (ret < 0) {
		if (__ratelimit(&read_ratelimit))
			pr_err("%s: ERROR read 0x%x fail %d\n",
				__func__, addr, ret);
	}

	*returnData = cmd_buf[1];

	return ret;
}
EXPORT_SYMBOL(lcd_panel_i2c_read_bytes);

int lcd_panel_i2c_write_multiple_bytes(unsigned char addr, unsigned char *value,
		unsigned int size)
{
	struct i2c_client *client = lcd_panel_i2c_client;
	static DEFINE_RATELIMIT_STATE(writes_ratelimit, 1 * HZ, 1);
	char *write_data = NULL;
	int ret = 0, i = 0;

	if (client == NULL) {
		pr_err("%s: ERROR!! lcd_panel_i2c_client is null\n", __func__);
		return 0;
	}

	if (IS_ERR_OR_NULL(value) || size == 0) {
		pr_err("%s: ERROR!! value is null, size:%u\n", __func__, size);
		return -EINVAL;
	}
	write_data = kzalloc(roundup(size + 1, 4), GFP_KERNEL);
	if (IS_ERR_OR_NULL(write_data)) {
		pr_err("%s: ERROR!! allocate buffer, size:%u\n", __func__, size);
		return -ENOMEM;
	}

	write_data[0] = addr;
	for (i = 0; i < size; i++)
		write_data[i + 1] = value[i];

	ret = i2c_master_send(client, write_data, size + 1);
	if (ret < 0) {
		if (__ratelimit(&writes_ratelimit))
			pr_err("%s: ERROR i2c write 0x%x fail %d\n",
				__func__, ret, addr);
	}

	kfree(write_data);
	write_data = NULL;
	return ret;
}
EXPORT_SYMBOL(lcd_panel_i2c_write_multiple_bytes);

static int lcd_panel_i2c_probe(struct i2c_client *client)
{
	pr_info("%s: name=%s addr=0x%x\n", __func__, client->name,
		 client->addr);
	lcd_panel_i2c_client = client;
	return 0;
}

static void lcd_panel_i2c_remove(struct i2c_client *client)
{
	pr_info("%s: name=%s addr=0x%x\n", __func__, client->name,
		 client->addr);
	lcd_panel_i2c_client = NULL;
	i2c_unregister_device(client);
}

static const struct of_device_id lcd_panel_i2c_of_match[] = {
	{.compatible = "mediatek,i2c_lcd_bias"},
	{},
};

static const struct i2c_device_id lcd_panel_i2c_id[] = {
	{LCD_PANEL_I2C_ID_NAME, 0},
	{},
};

struct i2c_driver lcd_panel_i2c_driver = {
	.id_table = lcd_panel_i2c_id,
	.probe = lcd_panel_i2c_probe,
	.remove = lcd_panel_i2c_remove,
	/* .detect		   = lcd_panel_i2c_detect, */
	.driver = {
		.owner = THIS_MODULE,
		.name = LCD_PANEL_I2C_ID_NAME,
		.of_match_table = lcd_panel_i2c_of_match,
	},
};

static int __init lcd_drm_bias_i2c_init(void)
{
	int ret = 0;

	pr_info("%s, add i2c driver\n", __func__);
	ret = i2c_add_driver(&lcd_panel_i2c_driver);
	if (ret < 0) {
		pr_err("%s, failed to register i2c driver: %d\n",
			  __func__, ret);
		return ret;
	}

	pr_info("%s-, ret:%d\n", __func__, ret);
	return 0;

}

static void __exit lcd_drm_bias_i2c_exit(void)
{
	i2c_del_driver(&lcd_panel_i2c_driver);
}
module_init(lcd_drm_bias_i2c_init);
module_exit(lcd_drm_bias_i2c_exit);


MODULE_AUTHOR("Cui Zhang <cui.zhang@mediatek.com>");
MODULE_DESCRIPTION("mediatek, panel i2c driver");
MODULE_LICENSE("GPL v2");
