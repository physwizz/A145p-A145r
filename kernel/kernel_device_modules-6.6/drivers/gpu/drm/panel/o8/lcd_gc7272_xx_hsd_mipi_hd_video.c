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
/*A06 code for AL7160A-1673 by yuli at 20240619 start*/
/*backlight from 46ma to 44ma*/
#define BACKLIGHT_MAX_REG_VAL   (4095*44)
#define BACKLIGHT_MAX_APP_VAL   (255*46)
/*A06 code for AL7160A-1673 by yuli at 20240619 end*/

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
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x55, 0xAA, 0x66);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x10);
    lcm_dcs_write_seq_static(ctx, 0x28);
    mdelay(50);
    lcm_dcs_write_seq_static(ctx, 0x10);
    mdelay(120);
    lcm_dcs_write_seq_static(ctx, 0x4F, 0x01);
    mdelay(10);

};

static void lcm_aot_suspend_setting(struct lcm *ctx)
{
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x55, 0xAA, 0x66);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x10);
    lcm_dcs_write_seq_static(ctx, 0x28);
    mdelay(50);
    lcm_dcs_write_seq_static(ctx, 0x10);
    mdelay(120);

};

static void lcm_panel_init(struct lcm *ctx)
{
    pr_info("%s+\n", __func__);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x55, 0xAA, 0x66);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x10);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x21);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x23);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x24);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x25);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x27);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x26);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x28);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xB3);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xA3);
    lcm_dcs_write_seq_static(ctx, 0xFB, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x2D, 0xAA);
    lcm_dcs_write_seq_static(ctx, 0xA3, 0x12);
    lcm_dcs_write_seq_static(ctx, 0xA7, 0x12);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xB3);
    lcm_dcs_write_seq_static(ctx, 0x4E, 0x46);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xB3);
    lcm_dcs_write_seq_static(ctx, 0x3F, 0x37);
    lcm_dcs_write_seq_static(ctx, 0x50, 0x10);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x29, 0xE0);
    lcm_dcs_write_seq_static(ctx, 0x20, 0x84);
    lcm_dcs_write_seq_static(ctx, 0x21, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x22, 0x80);
    lcm_dcs_write_seq_static(ctx, 0x2E, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xA3);
    lcm_dcs_write_seq_static(ctx, 0x58, 0xAA);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x26);
    lcm_dcs_write_seq_static(ctx, 0x43, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x0E, 0x33);
    lcm_dcs_write_seq_static(ctx, 0x0C, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x0B, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x28);
    lcm_dcs_write_seq_static(ctx, 0x7D, 0x1F);
    lcm_dcs_write_seq_static(ctx, 0x7B, 0x40);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xB3);
    lcm_dcs_write_seq_static(ctx, 0x47, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0xE4, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x01, 0x06);
    lcm_dcs_write_seq_static(ctx, 0x02, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x03, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0xC3, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xC4, 0xA7);
    lcm_dcs_write_seq_static(ctx, 0xC5, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xC6, 0xA7);
    lcm_dcs_write_seq_static(ctx, 0xB3, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xB6, 0xC8);
    lcm_dcs_write_seq_static(ctx, 0xB4, 0x10);
    lcm_dcs_write_seq_static(ctx, 0xB5, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xD3, 0x13);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x25, 0x06);
    lcm_dcs_write_seq_static(ctx, 0x26, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x2E, 0x6E);
    lcm_dcs_write_seq_static(ctx, 0x2F, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x36, 0x07);
    lcm_dcs_write_seq_static(ctx, 0x37, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x3F, 0x6E);
    lcm_dcs_write_seq_static(ctx, 0x40, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x28);
    lcm_dcs_write_seq_static(ctx, 0x01, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x02, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x03, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x04, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x05, 0x18);
    lcm_dcs_write_seq_static(ctx, 0x06, 0x19);
    lcm_dcs_write_seq_static(ctx, 0x07, 0x02);
    lcm_dcs_write_seq_static(ctx, 0x08, 0x21);
    lcm_dcs_write_seq_static(ctx, 0x09, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x0A, 0x08);
    lcm_dcs_write_seq_static(ctx, 0x0B, 0x0A);
    lcm_dcs_write_seq_static(ctx, 0x0C, 0x0C);
    lcm_dcs_write_seq_static(ctx, 0x0D, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x0E, 0x0E);
    lcm_dcs_write_seq_static(ctx, 0x0F, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x10, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x11, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x12, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x13, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x14, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x15, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x16, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x17, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x18, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x19, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x1A, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x1B, 0x18);
    lcm_dcs_write_seq_static(ctx, 0x1C, 0x19);
    lcm_dcs_write_seq_static(ctx, 0x1D, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x1E, 0x21);
    lcm_dcs_write_seq_static(ctx, 0x1F, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x20, 0x09);
    lcm_dcs_write_seq_static(ctx, 0x21, 0x0B);
    lcm_dcs_write_seq_static(ctx, 0x22, 0x0D);
    lcm_dcs_write_seq_static(ctx, 0x23, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x24, 0x0F);
    lcm_dcs_write_seq_static(ctx, 0x25, 0x01);
    lcm_dcs_write_seq_static(ctx, 0x26, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x27, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x28, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x29, 0x1A);
    lcm_dcs_write_seq_static(ctx, 0x2A, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x2B, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x2D, 0x25);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x28);
    lcm_dcs_write_seq_static(ctx, 0x30, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x31, 0x55);
    lcm_dcs_write_seq_static(ctx, 0x34, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x35, 0x55);
    lcm_dcs_write_seq_static(ctx, 0x36, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x37, 0x55);
    lcm_dcs_write_seq_static(ctx, 0x38, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x39, 0x0a);
    lcm_dcs_write_seq_static(ctx, 0x3A, 0x01);
    lcm_dcs_write_seq_static(ctx, 0x3B, 0x01);
    lcm_dcs_write_seq_static(ctx, 0x2F, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x21);
    lcm_dcs_write_seq_static(ctx, 0x7E, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x7F, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x8B, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x80, 0x02);
    lcm_dcs_write_seq_static(ctx, 0x8C, 0x19);
    lcm_dcs_write_seq_static(ctx, 0x81, 0x19);
    lcm_dcs_write_seq_static(ctx, 0x8D, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xAF, 0x70);
    lcm_dcs_write_seq_static(ctx, 0xB0, 0x70);
    lcm_dcs_write_seq_static(ctx, 0x83, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x8F, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x87, 0x21);
    lcm_dcs_write_seq_static(ctx, 0x93, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x82, 0x70);
    lcm_dcs_write_seq_static(ctx, 0x8E, 0x70);
    lcm_dcs_write_seq_static(ctx, 0x2B, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x2E, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x88, 0xB7);
    lcm_dcs_write_seq_static(ctx, 0x89, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x8A, 0x23);
    lcm_dcs_write_seq_static(ctx, 0x94, 0xB7);
    lcm_dcs_write_seq_static(ctx, 0x95, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x96, 0x23);
    lcm_dcs_write_seq_static(ctx, 0x45, 0x0f);
    lcm_dcs_write_seq_static(ctx, 0x46, 0x73);
    lcm_dcs_write_seq_static(ctx, 0x4C, 0x73);
    lcm_dcs_write_seq_static(ctx, 0x52, 0x73);
    lcm_dcs_write_seq_static(ctx, 0x58, 0x73);
    lcm_dcs_write_seq_static(ctx, 0x47, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x4D, 0x02);
    lcm_dcs_write_seq_static(ctx, 0x53, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x59, 0x21);
    lcm_dcs_write_seq_static(ctx, 0x48, 0x21);
    lcm_dcs_write_seq_static(ctx, 0x4E, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x54, 0x02);
    lcm_dcs_write_seq_static(ctx, 0x5a, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x76, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x77, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x78, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x79, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x49, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x4A, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x4F, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x50, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x55, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x56, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x5b, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x5c, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x84, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x90, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x85, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x91, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x08, 0x03);
    lcm_dcs_write_seq_static(ctx, 0xDC, 0xA0);
    lcm_dcs_write_seq_static(ctx, 0xE0, 0x80);
    lcm_dcs_write_seq_static(ctx, 0xE1, 0xA0);
    lcm_dcs_write_seq_static(ctx, 0xE2, 0x80);
    lcm_dcs_write_seq_static(ctx, 0xbe, 0x03);
    lcm_dcs_write_seq_static(ctx, 0xbf, 0x77);
    lcm_dcs_write_seq_static(ctx, 0xc0, 0x53);
    lcm_dcs_write_seq_static(ctx, 0xc1, 0x40);
    lcm_dcs_write_seq_static(ctx, 0xC2, 0x44);
    lcm_dcs_write_seq_static(ctx, 0xCA, 0x03);
    lcm_dcs_write_seq_static(ctx, 0xCB, 0x3c);
    lcm_dcs_write_seq_static(ctx, 0xCD, 0x3c);
    lcm_dcs_write_seq_static(ctx, 0xCC, 0x70);
    lcm_dcs_write_seq_static(ctx, 0xCE, 0x70);
    lcm_dcs_write_seq_static(ctx, 0xCF, 0x72);
    lcm_dcs_write_seq_static(ctx, 0xD0, 0x75);
    lcm_dcs_write_seq_static(ctx, 0xC4, 0x87);
    lcm_dcs_write_seq_static(ctx, 0xC5, 0x87);
    lcm_dcs_write_seq_static(ctx, 0x18, 0x40);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x05, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x08, 0x22);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x28);
    lcm_dcs_write_seq_static(ctx, 0x3D, 0x70);
    lcm_dcs_write_seq_static(ctx, 0x3E, 0x70);
    lcm_dcs_write_seq_static(ctx, 0x3F, 0x30);
    lcm_dcs_write_seq_static(ctx, 0x40, 0x30);
    lcm_dcs_write_seq_static(ctx, 0x45, 0x75);
    lcm_dcs_write_seq_static(ctx, 0x46, 0x75);
    lcm_dcs_write_seq_static(ctx, 0x47, 0x2c);
    lcm_dcs_write_seq_static(ctx, 0x48, 0x2c);
    lcm_dcs_write_seq_static(ctx, 0x4D, 0xA2);
    lcm_dcs_write_seq_static(ctx, 0x50, 0x2a);
    lcm_dcs_write_seq_static(ctx, 0x52, 0x71);
    lcm_dcs_write_seq_static(ctx, 0x53, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x56, 0x12);
    lcm_dcs_write_seq_static(ctx, 0x57, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x5A, 0x98);
    lcm_dcs_write_seq_static(ctx, 0x5B, 0x9C);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x7E, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x7F, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x80, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x81, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x82, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x83, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x84, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x85, 0x41);
    lcm_dcs_write_seq_static(ctx, 0x86, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x87, 0x41);
    lcm_dcs_write_seq_static(ctx, 0x88, 0x40);
    lcm_dcs_write_seq_static(ctx, 0x8A, 0x0A);
    lcm_dcs_write_seq_static(ctx, 0x8B, 0x0A);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x25);
    lcm_dcs_write_seq_static(ctx, 0x75, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x76, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x77, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x78, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x79, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x7A, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x7B, 0x64);
    lcm_dcs_write_seq_static(ctx, 0x7C, 0x32);
    lcm_dcs_write_seq_static(ctx, 0x7D, 0x9B);
    lcm_dcs_write_seq_static(ctx, 0x7E, 0x32);
    lcm_dcs_write_seq_static(ctx, 0x7F, 0x9B);
    lcm_dcs_write_seq_static(ctx, 0x80, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x81, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x82, 0x03);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x23);
    lcm_dcs_write_seq_static(ctx, 0xEF, 0x0B);
    lcm_dcs_write_seq_static(ctx, 0x29, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x01, 0x00, 0x22, 0x00, 0x34, 0x00, 0x4f, 0x00, 0x6c, 0x00, 0x84, 0x00, 0x9a, 0x00, 0xaa, 0x00, 0xbb);
    lcm_dcs_write_seq_static(ctx, 0x02, 0x00, 0xCa, 0x00, 0xFa, 0x01, 0x18, 0x01, 0x48, 0x01, 0x6F, 0x01, 0xA8, 0x01, 0xE3, 0x01, 0xE5);
    lcm_dcs_write_seq_static(ctx, 0x03, 0x02, 0x14, 0x02, 0x57, 0x02, 0x87, 0x02, 0xC8, 0x02, 0xF2, 0x03, 0x29, 0x03, 0x42, 0x03, 0x51);
    lcm_dcs_write_seq_static(ctx, 0x04, 0x03, 0x5C, 0x03, 0x79, 0x03, 0x97, 0x03, 0xB8, 0x03, 0xE8, 0x03, 0xFF);
    lcm_dcs_write_seq_static(ctx, 0x0D, 0x00, 0x22, 0x00, 0x34, 0x00, 0x4f, 0x00, 0x6c, 0x00, 0x84, 0x00, 0x9a, 0x00, 0xaa, 0x00, 0xbb);
    lcm_dcs_write_seq_static(ctx, 0x0E, 0x00, 0xCa, 0x00, 0xFa, 0x01, 0x18, 0x01, 0x48, 0x01, 0x6F, 0x01, 0xA8, 0x01, 0xE3, 0x01, 0xE5);
    lcm_dcs_write_seq_static(ctx, 0x0F, 0x02, 0x14, 0x02, 0x57, 0x02, 0x87, 0x02, 0xC8, 0x02, 0xF2, 0x03, 0x29, 0x03, 0x42, 0x03, 0x51);
    lcm_dcs_write_seq_static(ctx, 0x10, 0x03, 0x5C, 0x03, 0x79, 0x03, 0x97, 0x03, 0xB8, 0x03, 0xE8, 0x03, 0xFF);
    lcm_dcs_write_seq_static(ctx, 0xF0, 0x01);
    lcm_dcs_write_seq_static(ctx, 0x2D, 0x65);
    lcm_dcs_write_seq_static(ctx, 0x2E, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x2B, 0x41);
    lcm_dcs_write_seq_static(ctx, 0x32, 0x02);
    lcm_dcs_write_seq_static(ctx, 0x33, 0x18);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0xF1, 0xE0);
    lcm_dcs_write_seq_static(ctx, 0xF2, 0x45);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0xDC, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xE0, 0x01);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xA3);
    lcm_dcs_write_seq_static(ctx, 0x45, 0x11);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x20);
    lcm_dcs_write_seq_static(ctx, 0x4A, 0x04);
    lcm_dcs_write_seq_static(ctx, 0x24, 0x0A);
    lcm_dcs_write_seq_static(ctx, 0x48, 0x10);
    lcm_dcs_write_seq_static(ctx, 0x49, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x22);
    lcm_dcs_write_seq_static(ctx, 0x0D, 0x02);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0xB3);
    lcm_dcs_write_seq_static(ctx, 0x42, 0x05);
    lcm_dcs_write_seq_static(ctx, 0x43, 0x12);
    lcm_dcs_write_seq_static(ctx, 0x44, 0x52);
    lcm_dcs_write_seq_static(ctx, 0xFF, 0x10);
    lcm_dcs_write_seq_static(ctx, 0x51, 0x00, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x53, 0x2c);
    lcm_dcs_write_seq_static(ctx, 0x55, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x36, 0x08);
    lcm_dcs_write_seq_static(ctx, 0x69, 0x00);
    lcm_dcs_write_seq_static(ctx, 0x35, 0x00);
    lcm_dcs_write_seq_static(ctx, 0xBA, 0x03);
    lcm_dcs_write_seq_static(ctx, 0x11);
    mdelay(120);
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
    /*A06 code for AL7160A-4112 by guyuming at 20240910 start*/
    ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(ctx->reset_gpio)) {
        dev_err(ctx->dev, "cannot get lcd reset-gpios %ld\n", PTR_ERR(ctx->reset_gpio));
        return PTR_ERR(ctx->reset_gpio);
    }
    gpiod_set_value(ctx->reset_gpio, 1);
    /*A06 code for AL7160A-4112 by guyuming at 20240910 end*/
    mdelay(3);

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

    mdelay(10);

    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 0);
    mdelay(10);
    gpiod_set_value(ctx->reset_gpio, 1);
    mdelay(25);

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
#define HFP (315)
#define HSA (310)
#define HBP (300)
#define VAC (1600)
#define VFP (140)
#define VSA (6)
#define VBP (28)
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
    .pll_clk = 552,
    .cust_esd_check = 1,
    .esd_check_enable = 1,
    .lcm_esd_check_table[0] = {
        .cmd = 0x0a,
        .count = 1,
        .para_list[0] = 0x9C,
    },
    .physical_width_um = PHYSICAL_WIDTH,
    .physical_height_um = PHYSICAL_HEIGHT,
    .data_rate = 1104,

};

static int lcm_setbacklight_cmdq(void *dsi, dcs_write_gce cb, void *handle,
                 unsigned int level)
{

    char bl_tb0[] = {0x51, 0x0F, 0xF0};
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

    pr_info("%s+ lcm, gc7272_xx_hsd\n", __func__);

    if (!lcm_detect_panel("gc7272_xx_hsd")) {
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
        dev_err(dev, "cannot get lcd reset-gpios %ld\n", PTR_ERR(ctx->reset_gpio));
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

    pr_info("%s- lcm, gc7272_xx_hsd\n", __func__);

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
        .name = "lcd_gc7272_xx_hsd_mipi_hd_video",
        .owner = THIS_MODULE,
        .of_match_table = lcm_of_match,
    },
};

module_mipi_dsi_driver(lcm_driver);

MODULE_DESCRIPTION("lcm gc7272 video lcd Panel Driver");
MODULE_LICENSE("GPL v2");
