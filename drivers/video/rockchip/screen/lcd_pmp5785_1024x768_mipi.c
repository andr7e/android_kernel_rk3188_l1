#ifndef _LCD_YF__
#define _LCD_YF__

#include <linux/delay.h>
#include <mach/gpio.h>
#include "../transmitter/mipi_dsi.h"

// USE SSD2828_RGB2MIPI

#define OUT_TYPE        SCREEN_RGB
#define OUT_FACE        OUT_D888_P666

#define OUT_CLK         65000000
#define LCDC_ACLK       300000000

/* Timing */
#define H_PW            64
#define H_BP            56
#define H_VD            768
#define H_FP            60

#define V_PW            50
#define V_BP            30
#define V_VD            1024
#define V_FP            36

#define LCD_WIDTH       119
#define LCD_HEIGHT      159

#define DCLK_POL        1
#define SWAP_RB         0
#define SWAP_RG		0
#define SWAP_GB		0


#define mipi_dsi_init(data) 				dsi_set_regs(data, ARRAY_SIZE(data))
#define mipi_dsi_send_dcs_packet(data) 		dsi_send_dcs_packet(data, ARRAY_SIZE(data))
#define mipi_dsi_post_init(data)			dsi_set_regs(data, ARRAY_SIZE(data))
#define data_lane  4
static struct rk29lcd_info *gLcd_info = NULL;
int lcd_init(void);
int lcd_standby(u8 enable);

static unsigned int pre_initialize[] = {
	0x00B10000,
	0x00B20000,
	0x00B30000,
	0x00B40000,
	0x00B50000,
	0x00B60000 | (VPF_24BPP) | (VM_BM << 2),     // burst mode 24bits

	0x00de0000 | (data_lane -1),    //4 lanes
	0x00d60004,

	0x00B90000,
	0x00ba8016,   //pll
	0x00Bb0008,
	0x00B90001,
	0x00c40001,
};

static unsigned int post_initialize[] = {
	0x00B90000,
	
//	0x00ba8006,   //pll
//	0x00Bb0002,	
	0x00B7030b,
	
	0x00B90001,
	0x00B80000,
	0x00BC0000,
	0x00c00100,      //software reset ssd2828
};

static unsigned char mipi_exit_sleep_mode[]  = {0x11};
static unsigned char mipi_set_diaplay_on[]   = {0x29};
static unsigned char mipi_enter_sleep_mode[] = {0x10};
static unsigned char mipi_set_diaplay_off[]  = {0x28};

int lcd_init(void)
{
	printk("lcd_init...\n");
	
	dsi_probe_current_chip();
	gpio_direction_output(gLcd_info->reset_pin, 0);
	msleep(10);
	gpio_set_value(gLcd_info->reset_pin, 1);
	msleep(6);
	mipi_dsi_init(pre_initialize);
	msleep(1);
	mipi_dsi_send_dcs_packet(mipi_exit_sleep_mode);
	msleep(100);
	mipi_dsi_send_dcs_packet(mipi_set_diaplay_on);
	msleep(1);
	mipi_dsi_post_init(post_initialize);
    return 0;
}

int lcd_standby(u8 enable)
{
	if(enable) {
		printk("lcd_standby...\n");

		mipi_dsi_send_dcs_packet(mipi_set_diaplay_off);
		msleep(2);
		mipi_dsi_send_dcs_packet(mipi_enter_sleep_mode);
		msleep(100);

		dsi_power_off();
		gpio_set_value(gLcd_info->reset_pin, 0);
	} else {
		dsi_power_up();
		lcd_init();
	}
    return 0;
}

#define RK_USE_SCREEN_ID

static void set_lcd_info_by_id(struct rk29fb_screen *screen, struct rk29lcd_info *lcd_info )
{
	/* screen type & face */
	screen->type = OUT_TYPE;
	screen->face = OUT_FACE;
	screen->lvds_format = LVDS_8BIT_2;

	/* Screen size */
	screen->x_res = H_VD;
	screen->y_res = V_VD;

	screen->width  = LCD_WIDTH;
	screen->height = LCD_HEIGHT;

	/* Timing */
	screen->lcdc_aclk = LCDC_ACLK;
	screen->pixclock  = OUT_CLK;
	screen->left_margin  = H_BP;
	screen->right_margin = H_FP;
	screen->hsync_len    = H_PW;
	screen->upper_margin = V_BP;
	screen->lower_margin = V_FP;
	screen->vsync_len    = V_PW;

	/* Pin polarity */
	screen->pin_hsync = 0;
	screen->pin_vsync = 0;
	screen->pin_den   = 0;
	screen->pin_dclk  = DCLK_POL;

	/* Swap rule */
	screen->swap_rb = SWAP_RB;
	screen->swap_rg = SWAP_RG;
	screen->swap_gb = SWAP_GB;
	screen->swap_delta = 0;
	screen->swap_dumy  = 0;

	int vpw, hpw, vbp, hbp, vfp, hfp;
	
	screen->init = lcd_init;
	screen->standby = lcd_standby;
	if(lcd_info) {
		gLcd_info = lcd_info;
		gLcd_info->io_init();
	}
	
	vpw = screen->vsync_len;
	hpw = screen->hsync_len;
	vbp = screen->upper_margin;
	hbp = screen->left_margin;
	vfp = screen->lower_margin;
	hfp = screen->right_margin;
	
	pre_initialize[0] = 0x00B10000 | ((vpw & 0Xff) << 8) | (hpw & 0Xff);
	pre_initialize[1] = 0x00B20000 | (((vbp+vpw) & 0Xff) << 8) | ((hbp+hpw) & 0Xff);
	pre_initialize[2] = 0x00B30000 | ((vfp & 0Xff) << 8) | (hfp & 0Xff);
	pre_initialize[3] = 0x00B40000 | screen->x_res;
	pre_initialize[4] = 0x00B50000 | screen->y_res;
}
#endif
