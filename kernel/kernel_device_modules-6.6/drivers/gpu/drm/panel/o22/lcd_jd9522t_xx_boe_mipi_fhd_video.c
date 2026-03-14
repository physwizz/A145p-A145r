// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include <linux/backlight.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <drm/drm_modes.h>
#include <linux/delay.h>
#include <drm/drm_connector.h>
#include <drm/drm_device.h>

#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>
#include <video/of_videomode.h>
#include <video/videomode.h>

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/of_graph.h>
#include <linux/platform_device.h>

#define CONFIG_MTK_PANEL_EXT
#if defined(CONFIG_MTK_PANEL_EXT)
#include "../../mediatek/mediatek_v2/mtk_panel_ext.h"
#include "../../mediatek/mediatek_v2/mtk_drm_graphics_base.h"
#endif

extern bool g_smart_wakeup_flag;
extern bool lcm_detect_panel(const char *lcm_name);

/*a14 code for AL6528AV-239 by hehaoran at 20250208 start*/
extern int lcd_panel_i2c_write_bytes(unsigned char addr, unsigned char value);
/*a14 code for AL6528AV-239 by hehaoran at 20250208 end*/
#define BACKLIGHT_MAX_REG_VAL (4095/15*19)
#define BACKLIGHT_MAX_APP_VAL (255/15*23)

#include <linux/string.h>
static char gs_bl_tb0[3] = {0x51, 0x06, 0x66};//4096 * 40% = 1638


struct lcm {
    struct device *dev;
    struct drm_panel panel;
    struct backlight_device *backlight;
    struct gpio_desc *reset_gpio;
    struct gpio_desc *tpreset_gpio;
    struct gpio_desc *bias_pos;
    struct gpio_desc *bias_neg;
    bool prepared;
    bool enabled;

    unsigned int gate_ic;

    int error;
};

#define lcm_dcs_write_seq(ctx, seq...) \
({\
      const u8 d[] = { seq };\
      BUILD_BUG_ON_MSG(ARRAY_SIZE(d) > 64, "DCS sequence too big for stack");\
      lcm_dcs_write(ctx, d, ARRAY_SIZE(d));\
})

#define lcm_dcs_write_seq_static(ctx, seq...) \
({\
      static const u8 d[] = { seq };\
      lcm_dcs_write(ctx, d, ARRAY_SIZE(d));\
})

static inline struct lcm *panel_to_lcm(struct drm_panel *panel)
{
      return container_of(panel, struct lcm, panel);
}

static void lcm_dcs_write(struct lcm *ctx, const void *data, size_t len)
{
      struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
      ssize_t ret;
      char *addr;

      if (ctx->error < 0)
            return;

      addr = (char *)data;
      if ((int)*addr < 0xB0)
            ret = mipi_dsi_dcs_write_buffer(dsi, data, len);
      else
            ret = mipi_dsi_generic_write(dsi, data, len);
      if (ret < 0) {
            dev_err(ctx->dev, "error %zd writing seq: %ph\n", ret, data);
            ctx->error = ret;
      }
}

#ifdef PANEL_SUPPORT_READBACK
static int lcm_dcs_read(struct lcm *ctx, u8 cmd, void *data, size_t len)
{
      struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
      ssize_t ret;

      if (ctx->error < 0)
            return 0;

      ret = mipi_dsi_dcs_read(dsi, cmd, data, len);
      if (ret < 0) {
            dev_err(ctx->dev, "error %d reading dcs seq:(%#x)\n", ret, cmd);
            ctx->error = ret;
      }

      return ret;
}

static void lcm_panel_get_data(struct lcm *ctx)
{
      u8 buffer[3] = {0};
      static int ret;

      if (ret == 0) {
            ret = lcm_dcs_read(ctx,  0x0A, buffer, 1);
            dev_info(ctx->dev, "return %d data(0x%08x) to dsi engine\n",
                   ret, buffer[0] | (buffer[1] << 8));
      }
}
#endif

static void lcm_aot_suspend_setting(struct lcm *ctx)//lcm_aot_suspend_setting
{
    mdelay(5);
    lcm_dcs_write_seq_static(ctx, 0x28);
    mdelay(50);
    lcm_dcs_write_seq_static(ctx, 0x10);
    mdelay(120);

    lcm_dcs_write_seq_static(ctx, 0xBA, 0x01);
    mdelay(10);
};
static void lcm_suspend_setting(struct lcm *ctx)//lcm_suspend_setting
{
    mdelay(5);
    lcm_dcs_write_seq_static(ctx, 0x28);
    mdelay(50);
    lcm_dcs_write_seq_static(ctx, 0x10);
    mdelay(120);
};
static void lcm_sleep_out_setting(struct lcm *ctx)//lcm_sleep_out_setting
{
    lcm_dcs_write_seq_static(ctx, 0x11);
    mdelay(120);
    lcm_dcs_write_seq_static(ctx, 0x29);
    mdelay(10);
    lcm_dcs_write_seq_static(ctx, 0x51, 0x00, 0x01);
    lcm_dcs_write_seq_static(ctx, 0x55, 0x00);
    mdelay(5);
    lcm_dcs_write_seq_static(ctx, 0x53, 0x2C);
};

static void lcm_panel_init(struct lcm *ctx)
{
    pr_info("%s+\n", __func__);
    lcm_dcs_write_seq_static(ctx, 0xdf, 0x95, 0x25, 0xba);
    lcm_dcs_write_seq_static(ctx, 0xde, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xb9, 0x7c, 0x43, 0x85);
    lcm_dcs_write_seq_static(ctx, 0xbb, 0x00, 0x11, 0x43, 0x1f, 0x1f, 0x11, 0x11);
    lcm_dcs_write_seq_static(ctx, 0xbd, 0x00, 0x2d);
    lcm_dcs_write_seq_static(ctx, 0xbc, 0x2e, 0x30, 0x63);
    lcm_dcs_write_seq_static(ctx, 0xbf, 0x2c, 0x2c, 0x3f, 0xc3);
    lcm_dcs_write_seq_static(ctx, 0xc0, 0x00, 0xf7, 0x01, 0xf7, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xc1, 0x04, 0x11, 0x40);
    lcm_dcs_write_seq_static(ctx, 0xc3, 0x3b, 0x01, 0x06, 0x05, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x06, 0x2b, 0x30, 0x54, 0x58, 0x7d, 0x01, 0x80, 0xff, 0xff, 0x00, 0x05, 0x03, 0x7f, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x02, 0x13, 0xff, 0xff, 0x02, 0x13, 0x03, 0x00, 0x03, 0x02, 0x03, 0x03, 0x00, 0x03, 0xff, 0xff, 0x00, 0x07, 0xff, 0xff, 0x03, 0x0d, 0x03, 0x0d, 0x03, 0x0d, 0x03, 0x0d);
    lcm_dcs_write_seq_static(ctx, 0xc4, 0x01, 0x06, 0x05, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x06, 0x2b, 0x30, 0x54, 0x58, 0x7d, 0x01, 0x80, 0x03, 0x0d, 0x00, 0x05, 0x03, 0x0d, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x02, 0x13, 0x02, 0x13, 0x02, 0x13, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0xff, 0xff, 0x00, 0x07, 0x03, 0x0d, 0x03, 0x0d, 0x03, 0x0d, 0x03, 0x0d, 0x03, 0x0d);
    lcm_dcs_write_seq_static(ctx, 0xc6, 0x01, 0x4c, 0x00, 0x36, 0x00, 0x0a, 0x05, 0x82, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0x00, 0x00, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xc7, 0x40);
    lcm_dcs_write_seq_static(ctx, 0xc8, 0x45, 0xb4, 0x87, 0x00, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x3f, 0x3f);
    lcm_dcs_write_seq_static(ctx, 0xce, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xfc, 0xfc, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xcf, 0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xd0, 0x00, 0x1f, 0x1f, 0xde, 0xde, 0xdf, 0xdf, 0xf6, 0xf5, 0xf4, 0xf6, 0xf5, 0xf4, 0xdf, 0xc5, 0xc4, 0xc7, 0xc6, 0xd5, 0xfb, 0xfb, 0xde, 0xdf, 0xc1, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0);
    lcm_dcs_write_seq_static(ctx, 0xd1, 0x00, 0x1f, 0x1f, 0xde, 0xde, 0xdf, 0xdf, 0xf6, 0xf5, 0xf4, 0xf6, 0xf5, 0xf4, 0xdf, 0xc5, 0xc4, 0xc7, 0xc6, 0xd5, 0xfb, 0xfb, 0xde, 0xdf, 0xc1, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0);
    lcm_dcs_write_seq_static(ctx, 0xd2, 0x00, 0x1f, 0x1f, 0x1e, 0x1e, 0x1f, 0x1f, 0x36, 0x35, 0x34, 0x36, 0x35, 0x34, 0x1f, 0xc6, 0xc7, 0xc4, 0xc5, 0xd5, 0xfb, 0xfb, 0x1f, 0x1e, 0x00, 0x01, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xd3, 0x00, 0x1f, 0x1f, 0x1e, 0x1e, 0x1f, 0x1f, 0x36, 0x35, 0x34, 0x36, 0x35, 0x34, 0x1f, 0xc6, 0xc7, 0xc4, 0xc5, 0xd5, 0xfb, 0xfb, 0x1f, 0x1e, 0x00, 0x01, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xd4, 0x10, 0x10, 0x00, 0x08, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x11, 0xe0, 0x01, 0x00, 0x04, 0x01, 0x01, 0x11, 0xe0, 0x03, 0x00, 0x05, 0x01, 0x01, 0x00, 0x00, 0x02, 0x04, 0x0a, 0x09, 0x7c, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x26, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xd5, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0x00, 0x01, 0x00, 0x07, 0x32, 0x5a, 0x10, 0x40, 0x05, 0x00, 0x01, 0x00, 0x30, 0x30, 0x00, 0x01, 0x14, 0x00, 0x71, 0x00, 0x04, 0x10, 0x04, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 0x00, 0x2b, 0x00, 0x30, 0x00, 0x54, 0x00, 0x59, 0x00, 0x7d, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x3f, 0x3f, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00); 
    lcm_dcs_write_seq_static(ctx, 0xd7, 0x00, 0x02, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x84, 0x00, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xde, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xb5, 0x69);
    lcm_dcs_write_seq_static(ctx, 0xc1, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0xaa, 0xaf, 0xff, 0xf0, 0x00, 0x04, 0x17, 0x27, 0x37, 0x4a, 0x58, 0x66, 0x74, 0x89, 0x9d, 0xb3, 0xc4, 0xf9, 0x21, 0x43, 0x64, 0x7f, 0x9a, 0x9c, 0xb9, 0xd7, 0xfa, 0x22, 0x54, 0x95, 0xc1, 0xdb, 0xf9, 0x0a, 0x1e, 0x36, 0x50, 0x71, 0x9b, 0xd2, 0xfc);
    lcm_dcs_write_seq_static(ctx, 0xc2, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0xaa, 0xaf, 0xff, 0xf0, 0x00, 0x04, 0x17, 0x27, 0x37, 0x4a, 0x58, 0x66, 0x74, 0x89, 0x9d, 0xb3, 0xc4, 0xf9, 0x21, 0x43, 0x64, 0x7f, 0x9a, 0x9c, 0xb9, 0xd7, 0xfa, 0x22, 0x54, 0x95, 0xc1, 0xdb, 0xf9, 0x0a, 0x1e, 0x36, 0x50, 0x71, 0x9b, 0xd2, 0xfc);
    lcm_dcs_write_seq_static(ctx, 0xc3, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0xaa, 0xaf, 0xff, 0xf0, 0x00, 0x04, 0x17, 0x27, 0x37, 0x4a, 0x58, 0x66, 0x74, 0x89, 0x9d, 0xb3, 0xc4, 0xf9, 0x21, 0x43, 0x64, 0x7f, 0x9a, 0x9c, 0xb9, 0xd7, 0xfa, 0x22, 0x54, 0x95, 0xc1, 0xdb, 0xf9, 0x0a, 0x1e, 0x36, 0x50, 0x71, 0x9b, 0xd2, 0xfc);
    lcm_dcs_write_seq_static(ctx, 0xc4, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0xaa, 0xaf, 0xff, 0xf0, 0x00, 0x04, 0x17, 0x27, 0x37, 0x4a, 0x58, 0x66, 0x74, 0x89, 0x9d, 0xb3, 0xc4, 0xf9, 0x21, 0x43, 0x64, 0x7f, 0x9a, 0x9c, 0xb9, 0xd7, 0xfa, 0x22, 0x54, 0x95, 0xc1, 0xdb, 0xf9, 0x0a, 0x1e, 0x36, 0x50, 0x71, 0x9b, 0xd2, 0xfc);
    lcm_dcs_write_seq_static(ctx, 0xc5, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0xaa, 0xaf, 0xff, 0xf0, 0x00, 0x04, 0x17, 0x27, 0x37, 0x4a, 0x58, 0x66, 0x74, 0x89, 0x9d, 0xb3, 0xc4, 0xf9, 0x21, 0x43, 0x64, 0x7f, 0x9a, 0x9c, 0xb9, 0xd7, 0xfa, 0x22, 0x54, 0x95, 0xc1, 0xdb, 0xf9, 0x0a, 0x1e, 0x36, 0x50, 0x71, 0x9b, 0xd2, 0xfc);
    lcm_dcs_write_seq_static(ctx, 0xc6, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0xaa, 0xaf, 0xff, 0xf0, 0x00, 0x04, 0x17, 0x27, 0x37, 0x4a, 0x58, 0x66, 0x74, 0x89, 0x9d, 0xb3, 0xc4, 0xf9, 0x21, 0x43, 0x64, 0x7f, 0x9a, 0x9c, 0xb9, 0xd7, 0xfa, 0x22, 0x54, 0x95, 0xc1, 0xdb, 0xf9, 0x0a, 0x1e, 0x36, 0x50, 0x71, 0x9b, 0xd2, 0xfc);
    lcm_dcs_write_seq_static(ctx, 0xc7, 0x15, 0x14, 0x07, 0x07, 0x09, 0x07, 0x87, 0x63, 0x62, 0x63, 0x0c);
    lcm_dcs_write_seq_static(ctx, 0xc8, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff);
    lcm_dcs_write_seq_static(ctx, 0xde, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xbf, 0x04);
    lcm_dcs_write_seq_static(ctx, 0xc1, 0x20);
    lcm_dcs_write_seq_static(ctx, 0xc2, 0x02, 0x42, 0xd0, 0x02, 0x00, 0xc0, 0x41, 0x72, 0xfb);
    lcm_dcs_write_seq_static(ctx, 0xc0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99);
    lcm_dcs_write_seq_static(ctx, 0xc5, 0x0e);
    lcm_dcs_write_seq_static(ctx, 0xc4, 0x00, 0x11, 0x0f, 0x80, 0x1c, 0x01, 0x08);
    lcm_dcs_write_seq_static(ctx, 0xe5, 0x10, 0xe2, 0xe0, 0x02, 0x2b, 0xc0, 0x2b, 0xc0, 0x09, 0x04, 0x40, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xec, 0x77, 0x00, 0x5f, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xde, 0x03);
    lcm_dcs_write_seq_static(ctx, 0xd0, 0x00, 0x65);
    lcm_dcs_write_seq_static(ctx, 0xde, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x35, 0x00);

    pr_info("%s-\n", __func__);
}

static int lcm_disable(struct drm_panel *panel)
{
    struct lcm *ctx = panel_to_lcm(panel);
    pr_info("%s\n", __func__);

    if (!ctx->enabled)
        return 0;

    if (ctx->backlight) {
        ctx->backlight->props.power = FB_BLANK_POWERDOWN;
        backlight_update_status(ctx->backlight);
    }

    ctx->enabled = false;

    return 0;
}

static int lcm_unprepare(struct drm_panel *panel)
{

    struct lcm *ctx = panel_to_lcm(panel);

    if (!ctx->prepared)
        return 0;

    pr_info("%s+\n", __func__);

    if (g_smart_wakeup_flag) {
        lcm_suspend_setting(ctx);
    } else {
        lcm_aot_suspend_setting(ctx);

        ctx->bias_neg = devm_gpiod_get_index(ctx->dev, "bias", 1, GPIOD_OUT_LOW);
        if (IS_ERR(ctx->bias_neg)) {
            dev_err(ctx->dev, "%s: cannot get bias_neg %ld\n", __func__, PTR_ERR(ctx->bias_neg));
            return PTR_ERR(ctx->bias_neg);
        }
        gpiod_set_value(ctx->bias_neg, 0);
        devm_gpiod_put(ctx->dev, ctx->bias_neg);

        mdelay(5);

        ctx->bias_pos = devm_gpiod_get_index(ctx->dev, "bias", 0, GPIOD_OUT_LOW);
        if (IS_ERR(ctx->bias_pos)) {
            dev_err(ctx->dev, "%s: cannot get bias_pos %ld\n", __func__, PTR_ERR(ctx->bias_pos));
            return PTR_ERR(ctx->bias_pos);
        }
        gpiod_set_value(ctx->bias_pos, 0);
        devm_gpiod_put(ctx->dev, ctx->bias_pos);
    }

    ctx->error = 0;
    ctx->prepared = false;
    pr_info("%s-\n", __func__);
    return 0;
}

static int lcm_prepare(struct drm_panel *panel)
{
    struct lcm *ctx = panel_to_lcm(panel);
    int ret;

    pr_info("%s+\n", __func__);
    if (ctx->prepared)
        return 0;

    ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_LOW);
    if (IS_ERR(ctx->reset_gpio)) {
        dev_err(ctx->dev, "cannot get lcd reset-gpios %ld\n", PTR_ERR(ctx->reset_gpio));
        return PTR_ERR(ctx->reset_gpio);
    }
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(5);

    ctx->bias_pos = devm_gpiod_get_index(ctx->dev, "bias", 0, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_pos)) {
        dev_err(ctx->dev, "%s: cannot get bias_pos %ld\n", __func__, PTR_ERR(ctx->bias_pos));
        return PTR_ERR(ctx->bias_pos);
    }
    gpiod_set_value(ctx->bias_pos, 1);
    devm_gpiod_put(ctx->dev, ctx->bias_pos);
    /*a14 code for AL6528AV-239 by hehaoran at 20250208 start*/
    lcd_panel_i2c_write_bytes(0x0, 0xF);//bias_pos_addr:0x0   0xF *100mv + 4000mv = 5.5v
    /*a14 code for AL6528AV-239 by hehaoran at 20250208 end*/

    mdelay(5);

    ctx->bias_neg = devm_gpiod_get_index(ctx->dev, "bias", 1, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_neg)) {
        dev_err(ctx->dev, "%s: cannot get bias_neg %ld\n", __func__, PTR_ERR(ctx->bias_neg));
        return PTR_ERR(ctx->bias_neg);
    }
    gpiod_set_value(ctx->bias_neg, 1);
    devm_gpiod_put(ctx->dev, ctx->bias_neg);
    /*a14 code for AL6528AV-239 by hehaoran at 20250208 start*/
    lcd_panel_i2c_write_bytes(0x1, 0xF);//bias_pos_addr:0x1   0xF *100mv + 4000mv = 5.5v
    /*a14 code for AL6528AV-239 by hehaoran at 20250208 end*/

    mdelay(15);

    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(1);
    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(1);

    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(5);
    devm_gpiod_put(ctx->dev, ctx->reset_gpio);

    lcm_panel_init(ctx);

    mdelay(5);
    lcm_sleep_out_setting(ctx);

    ret = ctx->error;
    if (ret < 0)
        lcm_unprepare(panel);

    ctx->prepared = true;
#ifdef PANEL_SUPPORT_READBACK
    lcm_panel_get_data(ctx);
#endif

    pr_info("%s-\n", __func__);
    return ret;
}

static int lcm_enable(struct drm_panel *panel)
{
    struct lcm *ctx = panel_to_lcm(panel);
    pr_info("%s\n", __func__);

    if (ctx->enabled)
        return 0;

    if (ctx->backlight) {
        ctx->backlight->props.power = FB_BLANK_UNBLANK;
        backlight_update_status(ctx->backlight);
    }

    ctx->enabled = true;

    return 0;
}

#define HAC (1080)
#define HFP (36)
#define HSA (6)
#define HBP (36)
#define VAC (2408)
#define VFP (20)
#define VSA (2)
#define VBP (8)
#define PHYSICAL_WIDTH                  (68430)
#define PHYSICAL_HEIGHT                 (152570)
#define HTOTAL (HAC + HFP + HSA + HBP)
#define VTOTAL (VAC + VFP+  VSA + VBP)
#define CURRENT_FPS  60

static const struct drm_display_mode default_mode = {
    .clock = HTOTAL * VTOTAL * CURRENT_FPS / 1000, //.clock=htotal*vtotal*fps/1000
    .hdisplay = HAC,
    .hsync_start = HAC + HFP,
    .hsync_end = HAC + HFP + HSA,
    .htotal = HAC + HFP + HSA + HBP,
    .vdisplay = VAC,
    .vsync_start = VAC + VFP,
    .vsync_end = VAC + VFP + VSA,
    .vtotal = VAC + VFP + VSA + VBP,
    .width_mm = PHYSICAL_WIDTH / 1000,
    .height_mm = PHYSICAL_HEIGHT / 1000,
};

#if defined(CONFIG_MTK_PANEL_EXT)
static struct mtk_panel_params ext_params = {
    .pll_clk = 550,
    .cust_esd_check = 1,
    .esd_check_enable = 1,
    .lcm_esd_check_table[0] = {
        .cmd = 0x0a,
        .count = 1,
        .para_list[0] = 0x1C,
    },
    .physical_width_um = PHYSICAL_WIDTH,
    .physical_height_um = PHYSICAL_HEIGHT,
    .data_rate = 1104,

};
static int lcm_setbacklight_cmdq(void *dsi, dcs_write_gce cb, void *handle,
                 unsigned int level)
{

    char bl_tb0[] = {0x51, 0x0F, 0xFF};
    char bl_tb1[] = {0x53, 0x24};

    if (level == 0) {
        cb(dsi, handle, bl_tb1, ARRAY_SIZE(bl_tb1));
        pr_info("close diming\n");
    }
    pr_info("%s lcd_jd9522t_xx_boe_mipi_fhd_videoin level = 0x%x\n", __func__, level);
    level = mult_frac(level, BACKLIGHT_MAX_REG_VAL, BACKLIGHT_MAX_APP_VAL);
    pr_info("%s out level = 0x%x\n", __func__, level);
    bl_tb0[1] = level >> 8;
    bl_tb0[2] = level & 0xFF;
    if (level != 0) {
        memcpy(gs_bl_tb0,bl_tb0,sizeof(bl_tb0));
    }
    pr_info("%s bl_tb0[1] = 0x%x,bl_tb0[2]= 0x%x\n", __func__, bl_tb0[1], bl_tb0[2]);

    if (!cb) {
        return -1;
        pr_err("cb error\n");
    }

    cb(dsi, handle, bl_tb0, ARRAY_SIZE(bl_tb0));
    return 0;
}

static int panel_ext_reset(struct drm_panel *panel, int on)
{
    struct lcm *ctx = panel_to_lcm(panel);

    ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->reset_gpio)) {
        dev_err(ctx->dev, "cannot get lcd reset-gpios %ld\n", PTR_ERR(ctx->reset_gpio));
        return PTR_ERR(ctx->reset_gpio);
    }
    gpiod_set_value(ctx->reset_gpio, on);
    devm_gpiod_put(ctx->dev, ctx->reset_gpio);

    return 0;
}

static struct mtk_panel_funcs ext_funcs = {
    .reset = panel_ext_reset,
    .set_backlight_cmdq = lcm_setbacklight_cmdq,
};
#endif

struct panel_desc {
    const struct drm_display_mode *modes;
    unsigned int num_modes;

    unsigned int bpc;

    struct {
        unsigned int width;
        unsigned int height;
    } size;

    /**
     * @prepare: the time (in milliseconds) that it takes for the panel to
     *       become ready and start receiving video data
     * @enable: the time (in milliseconds) that it takes for the panel to
     *      display the first valid frame after starting to receive
     *      video data
     * @disable: the time (in milliseconds) that it takes for the panel to
     *       turn the display off (no content is visible)
     * @unprepare: the time (in milliseconds) that it takes for the panel
     *         to power itself down completely
     */
    struct {
        unsigned int prepare;
        unsigned int enable;
        unsigned int disable;
        unsigned int unprepare;
    } delay;
};

static int lcm_get_modes(struct drm_panel *panel,
                    struct drm_connector *connector)
{
    struct drm_display_mode *mode;

    mode = drm_mode_duplicate(connector->dev, &default_mode);
    if (!mode) {
        dev_info(connector->dev->dev, "failed to add mode %ux%ux@%u\n",
             default_mode.hdisplay, default_mode.vdisplay,
             drm_mode_vrefresh(&default_mode));
        return -ENOMEM;
    }

    drm_mode_set_name(mode);
    mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
    drm_mode_probed_add(connector, mode);

    connector->display_info.width_mm = PHYSICAL_WIDTH / 1000;
    connector->display_info.height_mm = PHYSICAL_HEIGHT / 1000;

    return 1;
}

static const struct drm_panel_funcs lcm_drm_funcs = {
    .disable = lcm_disable,
    .unprepare = lcm_unprepare,
    .prepare = lcm_prepare,
    .enable = lcm_enable,
    .get_modes = lcm_get_modes,
};

static int lcm_probe(struct mipi_dsi_device *dsi)
{
    struct device *dev = &dsi->dev;
    struct device_node *dsi_node, *remote_node = NULL, *endpoint = NULL;
    struct lcm *ctx;
    struct device_node *backlight;
    int ret;

    pr_info("%s+ lcm, jd9522t_xx_boe\n", __func__);

    if (!lcm_detect_panel("jd9522t_xx_boe")) {
        return -ENODEV;
    }

    pr_info("%s+ it is official\n", __func__);

    dsi_node = of_get_parent(dev->of_node);
    if (dsi_node) {
        endpoint = of_graph_get_next_endpoint(dsi_node, NULL);
        if (endpoint) {
            remote_node = of_graph_get_remote_port_parent(endpoint);
            if (!remote_node) {
                pr_err("No panel connected,skip probe lcm\n");
                return -ENODEV;
            }
            pr_info("device node name:%s\n", remote_node->name);
        }
    }
    if (remote_node != dev->of_node) {
        pr_err("%s+ skip probe due to not current lcm\n", __func__);
        return -ENODEV;
    }

    ctx = devm_kzalloc(dev, sizeof(struct lcm), GFP_KERNEL);
    if (!ctx)
        return -ENOMEM;

    mipi_dsi_set_drvdata(dsi, ctx);

    ctx->dev = dev;
    dsi->lanes = 4;
    dsi->format = MIPI_DSI_FMT_RGB888;
    dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
            MIPI_DSI_MODE_LPM | MIPI_DSI_MODE_NO_EOT_PACKET |
            MIPI_DSI_CLOCK_NON_CONTINUOUS;

    backlight = of_parse_phandle(dev->of_node, "backlight", 0);
    if (backlight) {
        ctx->backlight = of_find_backlight_by_node(backlight);
        of_node_put(backlight);

        if (!ctx->backlight)
            return -EPROBE_DEFER;
    }

    ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->reset_gpio)) {
        dev_info(dev, "cannot get lcd reset-gpios %ld\n",
             PTR_ERR(ctx->reset_gpio));
        return PTR_ERR(ctx->reset_gpio);
    }
    devm_gpiod_put(dev, ctx->reset_gpio);

    ctx->bias_pos = devm_gpiod_get_index(dev, "bias", 0, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_pos)) {
        dev_err(dev, "cannot get bias-gpios 0 %ld\n", PTR_ERR(ctx->bias_pos));
        return PTR_ERR(ctx->bias_pos);
    }
    devm_gpiod_put(dev, ctx->bias_pos);

    ctx->bias_neg = devm_gpiod_get_index(dev, "bias", 1, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_neg)) {
        dev_err(dev, "cannot get bias-gpios 1 %ld\n", PTR_ERR(ctx->bias_neg));
        return PTR_ERR(ctx->bias_neg);
    }
    devm_gpiod_put(dev, ctx->bias_neg);


    ctx->prepared = true;
    ctx->enabled = true;
    drm_panel_init(&ctx->panel, dev, &lcm_drm_funcs, DRM_MODE_CONNECTOR_DSI);

    drm_panel_add(&ctx->panel);

    ret = mipi_dsi_attach(dsi);
    if (ret < 0)
        drm_panel_remove(&ctx->panel);

#if defined(CONFIG_MTK_PANEL_EXT)
    mtk_panel_tch_handle_reg(&ctx->panel);
    ret = mtk_panel_ext_create(dev, &ext_params, &ext_funcs, &ctx->panel);
    if (ret < 0)
        return ret;

#endif

    pr_info("%s- lcm, jd9522t_xx_boe\n", __func__);

    return ret;
}

static void lcm_remove(struct mipi_dsi_device *dsi)
{
    struct lcm *ctx = mipi_dsi_get_drvdata(dsi);
#if defined(CONFIG_MTK_PANEL_EXT)
    struct mtk_panel_ctx *ext_ctx = find_panel_ctx(&ctx->panel);
#endif

    mipi_dsi_detach(dsi);
    drm_panel_remove(&ctx->panel);
#if defined(CONFIG_MTK_PANEL_EXT)
    mtk_panel_detach(ext_ctx);
    mtk_panel_remove(ext_ctx);
#endif
}

static const struct of_device_id lcm_of_match[] = {
    {
        .compatible = "panel,lcm,video",
    },
    {}
};

MODULE_DEVICE_TABLE(of, lcm_of_match);

static struct mipi_dsi_driver lcm_driver = {
    .probe = lcm_probe,
    .remove = lcm_remove,
    .driver = {
        .name = "lcd_jd9522t_xx_boe_mipi_fhd_video",
        .owner = THIS_MODULE,
        .of_match_table = lcm_of_match,
    },
};

module_mipi_dsi_driver(lcm_driver);

MODULE_DESCRIPTION("lcm jd9522t video lcd Panel Driver");
MODULE_LICENSE("GPL v2");
