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

/*A06_V code for AL7160AV-236 by hehaoran at 20250208 start*/
extern int lcd_panel_i2c_write_bytes(unsigned char addr, unsigned char value);
/*A06_V code for AL7160AV-236 by hehaoran at 20250208 end*/

/*backlight from 23ma to 20ma*/
#define BACKLIGHT_MAX_REG_VAL   (4095/15*23)
#define BACKLIGHT_MAX_APP_VAL   (255/15*23)

#include <linux/string.h>
static char gs_bl_tb0[3] = {0x51, 0x06, 0x66};//4096 * 40% = 1638

struct lcm {
    struct device *dev;
    struct drm_panel panel;
    struct backlight_device *backlight;
    struct gpio_desc *reset_gpio;
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

static void lcm_suspend_setting(struct lcm *ctx)
{
    lcm_dcs_write_seq_static(ctx, 0x28);
    mdelay(20);
    lcm_dcs_write_seq_static(ctx, 0x10);
    mdelay(120);
    lcm_dcs_write_seq_static(ctx, 0x4F, 0x00);
    mdelay(30);

};

static void lcm_aot_suspend_setting(struct lcm *ctx)
{
    lcm_dcs_write_seq_static(ctx, 0x28);
    mdelay(20);
    lcm_dcs_write_seq_static(ctx, 0x10);
    mdelay(120);

};

static void lcm_panel_init(struct lcm *ctx)
{
    pr_info("%s+\n", __func__);
    lcm_dcs_write_seq_static(ctx, 0xB9, 0x83, 0x10, 0x8B);
    lcm_dcs_write_seq_static(ctx, 0xBD, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xB1, 0x2C, 0x32, 0x32, 0x2F, 0x2F, 0x22, 0x43, 0x43, 0x1A, 0x1A, 0x0C, 0x98, 0x21, 0x11, 0x46, 0x73, 0x64, 0x24);
    lcm_dcs_write_seq_static(ctx, 0xB2, 0x00, 0x26, 0xD0, 0x40, 0x00, 0xE2, 0x3A, 0x20, 0x62, 0x11, 0x00, 0x00, 0x00, 0x54, 0x22, 0xDF);
    lcm_dcs_write_seq_static(ctx, 0xB4, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x49, 0x5A, 0x49, 0x02, 0x4E, 0x01, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xBA, 0x73, 0x03, 0xA8, 0x96);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xD0);
    lcm_dcs_write_seq_static(ctx, 0xBA, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xBC, 0x32);
    lcm_dcs_write_seq_static(ctx, 0xBF, 0xFC, 0x40);
    lcm_dcs_write_seq_static(ctx, 0xC0, 0x62, 0x62, 0x11, 0x22, 0xB3, 0xCF);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xC5);
    lcm_dcs_write_seq_static(ctx, 0xC7, 0x88, 0xCA, 0x08, 0x14, 0x02, 0x04, 0x04, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xC6);
    lcm_dcs_write_seq_static(ctx, 0xC8, 0x87, 0x13, 0xFF);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xCC, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xCB, 0x13, 0x08, 0xE0, 0x09, 0x57);
    lcm_dcs_write_seq_static(ctx, 0xD1, 0x27, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xD2, 0x00, 0x0F, 0x00, 0x0F);
    lcm_dcs_write_seq_static(ctx, 0xD3, 0xC0, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x5B, 0x5B, 0x66, 0x6B, 0xDA, 0xDA, 0xDA, 0xDA, 0x32, 0x10, 0xD6, 0x00, 0xD6, 0x32, 0x10, 0xDA, 0x00, 0xDA, 0x32, 0x10, 0x0F, 0x00, 0x0F, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xD5, 0x18, 0x18, 0x18, 0x18, 0x3E, 0x3E, 0x3E, 0x3E, 0x18, 0x18, 0x18, 0x18, 0xBE, 0xBE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xBE, 0xBE, 0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06, 0x09, 0x08, 0x0B, 0x0A, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x18, 0x18, 0x18, 0x18);
    lcm_dcs_write_seq_static(ctx, 0xD6, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3E, 0x3E, 0x3E, 0x3E, 0xBE, 0xBE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xBE, 0xBE, 0x02, 0x03, 0x00, 0x01, 0x0A, 0x0B, 0x08, 0x09, 0x06, 0x07, 0x04, 0x05, 0x24, 0x25, 0x20, 0x21, 0x22, 0x23, 0x18, 0x18, 0x18, 0x18);
    lcm_dcs_write_seq_static(ctx, 0xD8, 0xAA, 0xAE, 0xAE, 0xAA, 0xAA, 0xA0, 0xAA, 0xAE, 0xAE, 0xAA, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xE0, 0x00, 0x04, 0x0B, 0x12, 0x18, 0x28, 0x3D, 0x44, 0x4A, 0x47, 0x62, 0x69, 0x71, 0x82, 0x82, 0x8B, 0x95, 0xA8, 0xA7, 0x52, 0x5A, 0x65, 0x73, 0x00, 0x04, 0x0B, 0x12, 0x18, 0x28, 0x3D, 0x44, 0x4A, 0x47, 0x62, 0x69, 0x71, 0x82, 0x82, 0x8B, 0x95, 0xA8, 0xA7, 0x52, 0x5A, 0x65, 0x73);
    lcm_dcs_write_seq_static(ctx, 0xE7, 0xD3, 0x14, 0x14, 0x01, 0x0F, 0x84, 0x00, 0x3C, 0x3C, 0x32, 0x32, 0x00, 0x00, 0x00, 0x00, 0xE9, 0x10, 0x01, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xDD);
    lcm_dcs_write_seq_static(ctx, 0xE7, 0x32);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xE0);
    lcm_dcs_write_seq_static(ctx, 0xE7, 0x08);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xBD, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xB1, 0x01, 0x1B, 0x11);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xC6);
    lcm_dcs_write_seq_static(ctx, 0xBA, 0x33);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xBF, 0x0E);
    lcm_dcs_write_seq_static(ctx, 0xCB, 0x00, 0x34, 0x02, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xC6);
    lcm_dcs_write_seq_static(ctx, 0xD2, 0xD4, 0xEC);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xD3, 0x00, 0x04, 0x00, 0x28, 0x00, 0x00, 0x00, 0x81, 0x04);
    lcm_dcs_write_seq_static(ctx, 0xD8, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xE7, 0x01, 0x00, 0x48, 0x01, 0x48, 0x13, 0x48, 0x14, 0x00, 0x06, 0x1B, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xBD, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xBF, 0x7F, 0x00, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xD8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0xE0);
    lcm_dcs_write_seq_static(ctx, 0xE7, 0xD1);
    lcm_dcs_write_seq_static(ctx, 0xE9, 0x3F);
    lcm_dcs_write_seq_static(ctx, 0xBD, 0x03);
    lcm_dcs_write_seq_static(ctx, 0xD8, 0xAA, 0xAE, 0xAE, 0xAA, 0xAA, 0xA0, 0xAA, 0xAE, 0xAE, 0xAA, 0xAA, 0xA0, 0xAA, 0xAE, 0xAE, 0xAA, 0xAA, 0xA0, 0xAA, 0xAE, 0xAE, 0xAA, 0xAA, 0xA0);
    lcm_dcs_write_seq_static(ctx, 0xBD, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xC9, 0x00, 0x1E, 0x09, 0xEC, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xB9, 0x00, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x35, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x11);
    mdelay(120);
    lcm_dcs_write_seq_static(ctx, 0x53, 0x24);
    lcm_dcs_write_seq_static(ctx, 0x29);
    mdelay(20);
    lcm_dcs_write(ctx, gs_bl_tb0, sizeof(gs_bl_tb0));

    pr_info("%s-\n", __func__);
}

static int lcm_disable(struct drm_panel *panel)
{
    struct lcm *ctx = panel_to_lcm(panel);
    pr_info("%s+\n", __func__);

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
        lcm_aot_suspend_setting(ctx);
    } else {
        lcm_suspend_setting(ctx);

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
    /*A06_V code for AL7160AV-236 by hehaoran at 20250208 start*/
    lcd_panel_i2c_write_bytes(0x0, 0x14);
    /*A06_V code for AL7160AV-236 by hehaoran at 20250208 end*/

    mdelay(5);

    ctx->bias_neg = devm_gpiod_get_index(ctx->dev, "bias", 1, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_neg)) {
        dev_err(ctx->dev, "%s: cannot get bias_neg %ld\n", __func__, PTR_ERR(ctx->bias_neg));
        return PTR_ERR(ctx->bias_neg);
    }
    gpiod_set_value(ctx->bias_neg, 1);
    devm_gpiod_put(ctx->dev, ctx->bias_neg);
    /*A06_V code for AL7160AV-236 by hehaoran at 20250208 start*/
    lcd_panel_i2c_write_bytes(0x1, 0x14);
    /*A06_V code for AL7160AV-236 by hehaoran at 20250208 end*/

    mdelay(5);

    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(5);
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(2);
    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(50);

    devm_gpiod_put(ctx->dev, ctx->reset_gpio);

    lcm_panel_init(ctx);

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

#define HAC (720)
#define HFP (100)
#define HSA (20)
#define HBP (20)
#define VAC (1600)
#define VFP (1004)
#define VSA (8)
#define VBP (220)
#define PHYSICAL_WIDTH                  (70308)
#define PHYSICAL_HEIGHT                 (156240)
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
    .pll_clk = 504,
    .cust_esd_check = 1,
    .esd_check_enable = 1,
    .lcm_esd_check_table[0] = {
        .cmd = 0x0a,
        .count = 1,
        .para_list[0] = 0x9D,
    },
    .physical_width_um = PHYSICAL_WIDTH,
    .physical_height_um = PHYSICAL_HEIGHT,
    .data_rate = 1008,

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

    level = mult_frac(level, BACKLIGHT_MAX_REG_VAL, BACKLIGHT_MAX_APP_VAL);
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

    pr_info("%s+ lcm, hx83108b_xx_hkc\n", __func__);

    if (!lcm_detect_panel("hx83108b_xx_hkc")) {
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
            MIPI_DSI_MODE_LPM | MIPI_DSI_MODE_NO_EOT_PACKET | MIPI_DSI_MODE_VIDEO_BURST|
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
        dev_err(dev, "cannot get lcd reset-gpios %ld\n", PTR_ERR(ctx->reset_gpio));
        return PTR_ERR(ctx->reset_gpio);
    }
    devm_gpiod_put(dev, ctx->reset_gpio);

    // ctx->ctp_reset_gpio = devm_gpiod_get_index(dev, "reset", 1,GPIOD_OUT_HIGH);
    // if (IS_ERR(ctx->ctp_reset_gpio)) {
    //     dev_info(dev, "cannot get ctp reset-gpios %ld\n",
    //          PTR_ERR(ctx->ctp_reset_gpio));
    //     return PTR_ERR(ctx->ctp_reset_gpio);
    // }
    // devm_gpiod_put(dev, ctx->ctp_reset_gpio);

    ctx->bias_pos = devm_gpiod_get_index(dev, "bias", 0, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_pos)) {
        dev_info(dev, "cannot get bias-gpios 0 %ld\n",
                PTR_ERR(ctx->bias_pos));
        return PTR_ERR(ctx->bias_pos);
    }
    devm_gpiod_put(dev, ctx->bias_pos);

    ctx->bias_neg = devm_gpiod_get_index(dev, "bias", 1, GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->bias_neg)) {
        dev_info(dev, "cannot get bias-gpios 1 %ld\n",
                PTR_ERR(ctx->bias_neg));
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

    pr_info("%s- lcm, hx83108b_xx_hkc\n", __func__);

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
        .name = "lcd_hx83108b_xx_hkc_mipi_hd_video",
        .owner = THIS_MODULE,
        .of_match_table = lcm_of_match,
    },
};

module_mipi_dsi_driver(lcm_driver);

MODULE_DESCRIPTION("lcm hx83108b video lcd Panel Driver");
MODULE_LICENSE("GPL v2");
