/*
 * t0-sound.c - Sound Management of T0 Project
 *
 *  Copyright (C) 2012 Samsung Electrnoics
 *  Uk Kim <w0806.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/i2c.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/i2c-gpio.h>
#include <mach/irqs.h>
#include <mach/pmu.h>
#include <plat/iic.h>

#include <plat/gpio-cfg.h>
#include <mach/gpio-midas.h>

#ifdef CONFIG_SND_SOC_WM8994
#include <linux/mfd/wm8994/pdata.h>
#include <linux/mfd/wm8994/gpio.h>
#endif

#include <linux/exynos_audio.h>

static bool midas_snd_mclk_enabled;

#define I2C_NUM_CODEC	4
#define SET_PLATDATA_CODEC(i2c_pd)	s3c_i2c4_set_platdata(i2c_pd)

static DEFINE_SPINLOCK(midas_snd_spinlock);

void midas_snd_set_mclk(bool on, bool forced)
{
	static int use_cnt;

	spin_lock(&midas_snd_spinlock);

	midas_snd_mclk_enabled = on;

	if (midas_snd_mclk_enabled) {
		if (use_cnt++ == 0 || forced) {
			pr_info("Sound: enabled mclk\n");
			exynos4_pmu_xclkout_set(midas_snd_mclk_enabled,
							XCLKOUT_XUSBXTI);
			mdelay(10);
		}
	} else {
		if ((--use_cnt <= 0) || forced) {
			pr_info("Sound: disabled mclk\n");
#ifdef CONFIG_ARCH_EXYNOS5
			exynos5_pmu_xclkout_set(midas_snd_mclk_enabled,
							XCLKOUT_XXTI);
#else /* for CONFIG_ARCH_EXYNOS4 */
			exynos4_pmu_xclkout_set(midas_snd_mclk_enabled,
							XCLKOUT_XUSBXTI);
#endif
			use_cnt = 0;
		}
	}

	spin_unlock(&midas_snd_spinlock);

	pr_info("Sound: state: %d, use_cnt: %d\n",
					midas_snd_mclk_enabled, use_cnt);
}

bool midas_snd_get_mclk(void)
{
	return midas_snd_mclk_enabled;
}

#ifdef CONFIG_SND_SOC_WM8994
/* vbatt_devices */
static struct regulator_consumer_supply vbatt_supplies[] = {
	REGULATOR_SUPPLY("LDO1VDD", NULL),
	REGULATOR_SUPPLY("SPKVDD1", NULL),
	REGULATOR_SUPPLY("SPKVDD2", NULL),
};

static struct regulator_init_data vbatt_initdata = {
	.constraints = {
		.always_on = 1,
	},
	.num_consumer_supplies = ARRAY_SIZE(vbatt_supplies),
	.consumer_supplies = vbatt_supplies,
};

static struct fixed_voltage_config vbatt_config = {
	.init_data = &vbatt_initdata,
	.microvolts = 5000000,
	.supply_name = "VBATT",
	.gpio = -EINVAL,
};

struct platform_device vbatt_device = {
	.name = "reg-fixed-voltage",
	.id = -1,
	.dev = {
		.platform_data = &vbatt_config,
	},
};

/* wm1811 ldo1 */
static struct regulator_consumer_supply wm1811_ldo1_supplies[] = {
	REGULATOR_SUPPLY("AVDD1", NULL),
};

static struct regulator_init_data wm1811_ldo1_initdata = {
	.constraints = {
		.name = "WM1811 LDO1",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(wm1811_ldo1_supplies),
	.consumer_supplies = wm1811_ldo1_supplies,
};

/* wm1811 ldo2 */
static struct regulator_consumer_supply wm1811_ldo2_supplies[] = {
	REGULATOR_SUPPLY("DCVDD", NULL),
};

static struct regulator_init_data wm1811_ldo2_initdata = {
	.constraints = {
		.name = "WM1811 LDO2",
		.always_on = true, /* Actually status changed by LDO1 */
	},
	.num_consumer_supplies = ARRAY_SIZE(wm1811_ldo2_supplies),
	.consumer_supplies = wm1811_ldo2_supplies,
};

static struct wm8994_drc_cfg drc_value[] = {
	{
		.name = "voice call DRC",
		.regs[0] = 0x009B,
		.regs[1] = 0x0844,
		.regs[2] = 0x00E8,
		.regs[3] = 0x0210,
		.regs[4] = 0x0000,
	},
};

static struct wm8994_pdata wm1811_pdata = {
	.gpio_defaults = {
		[0] = WM8994_GP_FN_IRQ,	  /* GPIO1 IRQ output, CMOS mode */
		[7] = WM8994_GPN_DIR | WM8994_GP_FN_PIN_SPECIFIC, /* DACDAT3 */
		[8] = WM8994_CONFIGURE_GPIO |
		      WM8994_GP_FN_PIN_SPECIFIC, /* ADCDAT3 */
		[9] = WM8994_CONFIGURE_GPIO |\
		      WM8994_GP_FN_PIN_SPECIFIC, /* LRCLK3 */
		[10] = WM8994_CONFIGURE_GPIO |\
		       WM8994_GP_FN_PIN_SPECIFIC, /* BCLK3 */
	},

	.irq_base = IRQ_BOARD_CODEC_START,

	/* The enable is shared but assign it to LDO1 for software */
	.ldo = {
		{
			.enable = GPIO_WM8994_LDO,
			.init_data = &wm1811_ldo1_initdata,
		},
		{
			.init_data = &wm1811_ldo2_initdata,
		},
	},
	/* Apply DRC Value */
	.drc_cfgs = drc_value,
	.num_drc_cfgs = ARRAY_SIZE(drc_value),

	/* Support external capacitors*/
	.jd_ext_cap = 1,

	/* Regulated mode at highest output voltage */
	.micbias = {0x2f, 0x27},

	.micd_lvl_sel = 0xFF,

	.ldo_ena_always_driven = true,
	.ldo_ena_delay = 30000,

	.lineout1fb = 1,

	.lineout2fb = 0,
};

static struct i2c_board_info i2c_wm1811[] __initdata = {
	{
		I2C_BOARD_INFO("wm1811", (0x34 >> 1)),	/* Audio CODEC */
		.platform_data = &wm1811_pdata,
		.irq = IRQ_EINT(30),
	},
};

#endif

static void t0_gpio_init(void)
{
	int err;
	unsigned int gpio;

#ifdef CONFIG_SND_SOC_USE_EXTERNAL_MIC_BIAS
	/* Main Microphone BIAS */
	err = gpio_request(GPIO_MIC_BIAS_EN, "MAIN MIC");
	if (err) {
		pr_err(KERN_ERR "MIC_BIAS_EN GPIO set error!\n");
		return;
	}
	gpio_direction_output(GPIO_MIC_BIAS_EN, 1);
	gpio_set_value(GPIO_MIC_BIAS_EN, 0);
	gpio_free(GPIO_MIC_BIAS_EN);
#endif

#ifdef CONFIG_SND_USE_SUB_MIC
	/* Sub Microphone BIAS */
	err = gpio_request(GPIO_SUB_MIC_BIAS_EN, "SUB MIC");
	if (err) {
		pr_err(KERN_ERR "SUB_MIC_BIAS_EN GPIO set error!\n");
		return;
	}
	gpio_direction_output(GPIO_SUB_MIC_BIAS_EN, 1);
	gpio_set_value(GPIO_SUB_MIC_BIAS_EN, 0);
	gpio_free(GPIO_SUB_MIC_BIAS_EN);
#endif

#ifdef CONFIG_SND_USE_LINEOUT_SWITCH
	err = gpio_request(GPIO_VPS_SOUND_EN, "LINEOUT_EN");
	if (err) {
		pr_err(KERN_ERR "LINEOUT_EN GPIO set error!\n");
		return;
	}
	gpio_direction_output(GPIO_VPS_SOUND_EN, 0);
	gpio_set_value(GPIO_VPS_SOUND_EN, 0);
	gpio_free(GPIO_VPS_SOUND_EN);
#endif

	if (system_rev >= 3)
		gpio = GPIO_G_DET_N_REV03;
	else
		gpio = GPIO_G_DET_N;

	err = gpio_request(gpio, "GROUND DET");
	if (err) {
		pr_err(KERN_ERR "G_DET_N GPIO set error!\n");
		return;
	}
	gpio_direction_input(gpio);
	gpio_free(gpio);

#ifdef CONFIG_JACK_FET
	if (system_rev >= 4) {
		err = gpio_request(GPIO_EAR_BIAS_DISCHARGE, "EAR DISCHARGE");
		if (err) {
			pr_err("EAR_BIAS_DISCHARGE GPIO  set error!\n");
			return;
		}
		gpio_direction_output(GPIO_EAR_BIAS_DISCHARGE, 0);
		gpio_free(GPIO_EAR_BIAS_DISCHARGE);
	}

#endif
}

static void t0_set_lineout_switch(int on)
{
#ifdef CONFIG_SND_USE_LINEOUT_SWITCH
#ifndef CONFIG_MACH_T0_USA_SPR
	if (system_rev < 3)
		return;
#endif

	gpio_set_value(GPIO_VPS_SOUND_EN, on);
	pr_info("%s: lineout switch on = %d\n", __func__, on);
#endif
}

static void t0_set_ext_main_mic(int on)
{
#ifdef CONFIG_SND_SOC_USE_EXTERNAL_MIC_BIAS
	/* Main Microphone BIAS */
	gpio_set_value(GPIO_MIC_BIAS_EN, on);
	pr_info("%s: main_mic bias on = %d\n", __func__, on);
#endif
}

static void t0_set_ext_sub_mic(int on)
{
#ifdef CONFIG_SND_USE_SUB_MIC
	/* Sub Microphone BIAS */
	gpio_set_value(GPIO_SUB_MIC_BIAS_EN, on);
	pr_info("%s: sub_mic bias on = %d\n", __func__, on);
#endif
}

static int t0_get_ground_det_value(void)
{
	unsigned int g_det_gpio;

	if (system_rev >= 3)
		g_det_gpio = GPIO_G_DET_N_REV03;
	else
		g_det_gpio = GPIO_G_DET_N;
	return gpio_get_value(g_det_gpio);
}

struct exynos_sound_platform_data t0_sound_pdata __initdata = {
	.set_lineout_switch	= t0_set_lineout_switch,
	.set_ext_main_mic	= t0_set_ext_main_mic,
	.set_ext_sub_mic	= t0_set_ext_sub_mic,
	.get_ground_det_value	= t0_get_ground_det_value,
};

void __init midas_sound_init(void)
{
	pr_info("Sound: start %s\n", __func__);

	t0_gpio_init();

	pr_info("%s: set sound platform data for T0 device\n", __func__);
	if (exynos_sound_set_platform_data(&t0_sound_pdata))
		pr_err("%s: failed to register sound pdata\n", __func__);

	SET_PLATDATA_CODEC(NULL);
	i2c_register_board_info(I2C_NUM_CODEC, i2c_wm1811,
					ARRAY_SIZE(i2c_wm1811));

}
