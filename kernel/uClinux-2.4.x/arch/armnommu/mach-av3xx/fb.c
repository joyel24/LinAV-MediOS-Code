/*
 * fb.c - Frame-buffer driver for iPod
 *
 * Copyright (c) 2003,2004 Bernard Leach (leachbj@bouncycastle.org)
 *
 * The LCD uses the HD66753 controller from Hitachi (now owned by Renesas).
 */

#include <linux/config.h>
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/ioctl.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <video/fbcon.h>
#include <video/fbcon-cfb8.h>
#include <video/fbcon-cfb32.h>

#include <asm/arch/av3xx_osd.h>
#include <asm/arch/av3xx_pallette.h>

#define AV3XX_LCD_WIDTH   320
#define AV3XX_LCD_HEIGHT  240

/* allow for 8bpp */
//static char av3xx_fb[AV3XX_LCD_HEIGHT * AV3XX_LCD_WIDTH];
extern char av3xx_fb[AV3XX_LCD_HEIGHT * AV3XX_LCD_WIDTH ];

#if 0

static char av3xx_scr[AV3XX_LCD_HEIGHT * AV3XX_LCD_WIDTH];

static void av3xx_update_display(struct display *p, int sx, int sy, int mx, int my)
{
	int x,y;

	#if 0
	int offset=sy * fontheight(p) * p->line_length ;

	for ( y = sy * fontheight(p); y < my * fontheight(p); y++ ) {
		for ( x = sx; x < mx; x++ ) {
			/* display a character */
				av3xx_scr[offset+x ]=av3xx_fb[offset+x ];

		}
		offset+=p->line_length ;
	}
	#endif
	int offset=0;
	for ( y = 0; y < AV3XX_LCD_HEIGHT; y++ ) {
		for ( x = 0; x < AV3XX_LCD_WIDTH; x++ )
			av3xx_scr[offset+x ]=av3xx_fb[offset+x ];
		offset+=p->line_length ;
	}

}
#endif

void init_lcd()
{
	osdInit();

    osdSetComponentConfig(AV3XX_OSD_VIDEO1, 0);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2, 0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR1, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR2, 0);

    osdSetComponentSize(AV3XX_OSD_BITMAP1, 320*2, 240);
    osdSetComponentPosition(AV3XX_OSD_BITMAP1,0x14, 0x12);
    osdSetComponentOffset(AV3XX_OSD_BITMAP1, (int)av3xx_fb);
    osdSetComponentSourceWidth(AV3XX_OSD_BITMAP1, 0xa);
    osdSetComponentConfig(AV3XX_OSD_BITMAP1, AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                                 AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE);

    /*osdSetComponentSize(AV3XX_OSD_VIDEO1, 320*2, 240);
    osdSetComponentPosition(AV3XX_OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffset(AV3XX_OSD_VIDEO1, (int)av3xx_fb);
    osdSetComponentSourceWidth(AV3XX_OSD_VIDEO1, 0x28);
    osdSetComponentConfig(AV3XX_OSD_VIDEO1,  AV3XX_OSD_COMPONENT_ENABLE);*/

}

static unsigned get_contrast(void)
{
	/* oxygen 77 to be done */

	return 0x4;
}

static void set_contrast(int contrast)
{
	/* oxygen 77 to be done */
}

static int get_backlight(void)
{
	return 0x4;
}


static void set_backlight(int on)
{
	/* oxygen 77 to be done */

}

struct av3xxfb_info {
	/*
	 *  Choose _one_ of the two alternatives:
	 *
	 *    1. Use the generic frame buffer operations (fbgen_*).
	 */
	struct fb_info_gen gen;

#if 0
	/*
	 *    2. Provide your own frame buffer operations.
	 */
	struct fb_info info;
#endif

	/* Here starts the frame buffer device dependent part */
	/* You can use this to store e.g. the board number if you support */
	/* multiple boards */
};


struct av3xxfb_par {
	/*
	 *  The hardware specific data in this structure uniquely defines a video
	 *  mode.
	 *
	 *  If your hardware supports only one video mode, you can leave it empty.
	 */
};

/*
void av3xx_fb_setup(struct display *p)
{
	fbcon_cfb8.setup(p);
}

void av3xx_fb_bmove(struct display *p, int sy, int sx, int dy, int dx,
		     int height, int width)
{
	fbcon_cfb8.bmove(p, sy, sx, dy, dx, height, width);
	//av3xx_update_display(p, 0, 0, AV3XX_LCD_WIDTH, AV3XX_LCD_HEIGHT/fontheight(p));
}

void av3xx_fb_clear(struct vc_data *conp, struct display *p, int sy, int sx,
		     int height, int width)
{
	fbcon_cfb8.clear(conp, p, sy, sx, height, width);
	//av3xx_update_display(p, sx, sy, sx+width, sy+height);
}

void av3xx_fb_putc(struct vc_data *conp, struct display *p, int c, int yy,
		    int xx)
{
	fbcon_cfb8.putc(conp, p, c, yy, xx);
	//av3xx_update_display(p, xx, yy, xx+1, yy+1);
}

void av3xx_fb_putcs(struct vc_data *conp, struct display *p, 
		     const unsigned short *s, int count, int yy, int xx)
{
	fbcon_cfb8.putcs(conp, p, s, count, yy, xx);
	//av3xx_update_display(p, xx, yy, xx+count, yy+1);
}

void av3xx_fb_revc(struct display *p, int xx, int yy)
{
	fbcon_cfb8.revc(p, xx, yy);
	//av3xx_update_display(p, xx, yy, xx+1, yy+1);
}
*/

/*
 *  `switch' for the low level operations
 */
/*
struct display_switch fbcon_av3xx = {
	setup:           av3xx_fb_setup,
	bmove:           av3xx_fb_bmove,
	clear:           av3xx_fb_clear,
	putc:            av3xx_fb_putc,
	putcs:           av3xx_fb_putcs,
	revc:            av3xx_fb_revc,
	fontwidthmask:   FONTWIDTH(8)
};
*/
static struct av3xxfb_info fb_info;
static struct av3xxfb_par current_par;
static int current_par_valid = 0;
static struct display disp;

static struct fb_var_screeninfo default_var;

int av3xxfb_init(void);
int av3xxfb_setup(char*);

/* ------------------- chipset specific functions -------------------------- */

static void av3xx_get_par(struct av3xxfb_par *, const struct fb_info *);
static int av3xx_encode_var(struct fb_var_screeninfo *, struct av3xxfb_par *, const struct fb_info *);

static void av3xx_detect(void)
{
	/*
	 *  This function should detect the current video mode settings and store
	 *  it as the default video mode
	 */

	struct av3xxfb_par par;

	av3xx_get_par(&par, NULL);
	av3xx_encode_var(&default_var, &par, NULL);
}

static int av3xx_encode_fix(struct fb_fix_screeninfo *fix, struct av3xxfb_par *par,
			  const struct fb_info *info)
{
	/*
	 *  This function should fill in the 'fix' structure based on the values
	 *  in the `par' structure.
	 */

	memset(fix, 0x0, sizeof(*fix));

	strcpy(fix->id, "Av3xx");
	/* required for mmap() */
	fix->smem_start = av3xx_fb;
	fix->smem_len = AV3XX_LCD_HEIGHT * (AV3XX_LCD_WIDTH); // depends on bpp here it's 8=>*1 32=>*4

	fix->type= FB_TYPE_PACKED_PIXELS;

	fix->visual = FB_VISUAL_PSEUDOCOLOR;//FB_VISUAL_TRUECOLOR;	/* fixed visual */
	fix->line_length = AV3XX_LCD_WIDTH ;	/* cfb2 default  8=>*1 32=>*4*/

	fix->xpanstep = 0;	/* no hardware panning */
	fix->ypanstep = 0;	/* no hardware panning */
	fix->ywrapstep = 0;	/* */

	fix->accel = FB_ACCEL_NONE;

	return 0;
}

static int av3xx_decode_var(struct fb_var_screeninfo *var, struct av3xxfb_par *par,
			  const struct fb_info *info)
{
	/*
	 *  Get the video params out of 'var'. If a value doesn't fit, round it up,
	 *  if it's too big, return -EINVAL.
	 *
	 *  Suggestion: Round up in the following order: bits_per_pixel, xres,
	 *  yres, xres_virtual, yres_virtual, xoffset, yoffset, grayscale,
	 *  bitfields, horizontal timing, vertical timing.
	 */

	if ( var->xres > AV3XX_LCD_WIDTH ||
		var->yres > AV3XX_LCD_HEIGHT ||
		var->xres_virtual != var->xres ||
		var->yres_virtual != var->yres ||
		var->xoffset != 0 ||
		var->yoffset != 0 ) {
		return -EINVAL;
	}

	if ( var->bits_per_pixel != 8 ) {  /* 8 or 32 */
		return -EINVAL;
	}

	return 0;
}

static int av3xx_encode_var(struct fb_var_screeninfo *var, struct av3xxfb_par *par,
			  const struct fb_info *info)
{
	/*
	 *  Fill the 'var' structure based on the values in 'par' and maybe other
	 *  values read out of the hardware.
	 */

	var->xres = AV3XX_LCD_WIDTH;
	var->yres = AV3XX_LCD_HEIGHT;
	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres;
	var->xoffset = 0;
	var->yoffset = 0;

	var->bits_per_pixel = 8; /* 8 or 32 */
	var->grayscale = 0;

	return 0;
}

static void av3xx_get_par(struct av3xxfb_par *par, const struct fb_info *info)
{
	/*
	 *  Fill the hardware's 'par' structure.
	 */

	if ( current_par_valid ) {
		*par = current_par;
	}
	else {
		/* ... */
	}
}

static void av3xx_set_par(struct av3xxfb_par *par, const struct fb_info *info)
{
	/*
	 *  Set the hardware according to 'par'.
	 */

	current_par = *par;
	current_par_valid = 1;

	/* ... */
}

static int av3xx_getcolreg(unsigned regno, unsigned *red, unsigned *green,
			 unsigned *blue, unsigned *transp,
			 const struct fb_info *info)
{
	/*
	 *  Read a single color register and split it into colors/transparent.
	 *  The return values must have a 16 bit magnitude.
	 *  Return != 0 for invalid regno.
	 */
	 if(regno<256)
	 {
	 	*red=(av3xx_pallette_sav[regno][0])<<8;
		*green=(av3xx_pallette_sav[regno][1])<<8;
		*blue=(av3xx_pallette_sav[regno][2])<<8;
	 }
	 else
		return -1;
	//printk("getting palette: %02x\n",regno);
	/* ... */
	return 0;
}

static int av3xx_setcolreg(unsigned regno, unsigned red, unsigned green,
			 unsigned blue, unsigned transp,
			 const struct fb_info *info)
{
	/*
	 *  Set a single color register. The values supplied have a 16 bit
	 *  magnitude.
	 *  Return != 0 for invalid regno.
	 */
	int y,cr,cb;
	/*red&=0xFF;
	green&=0xFF;
	blue&=0xFF;*/
	red=(red>>8)&0xff;
	green=(green>>8)&0xff;
	blue=(blue>>8)&0xff;
	
	
	
	if(regno<256)
	{
		av3xx_pallette_sav[regno][0]=red;
		av3xx_pallette_sav[regno][1]=green;
		av3xx_pallette_sav[regno][2]=blue;
	}
	else
		return -1;
	
	y = (306*red + 601*green + 117*blue) >> 10 ; // +16 ??
	cb = ((-173*red -339*green + 512*blue) >> 10) + 128;
	cr = ((512*red - 429*green - 83*blue) >> 10) + 128;

	osdSetPallette (y, cr, cb, regno);
	
	//printk("setting palette: r=%02x g=%02x b=%02x (%02x)\n",red,green,blue,regno);
	
	return 0;
}

static int av3xx_pan_display(struct fb_var_screeninfo *var,
			   struct av3xxfb_par *par, const struct fb_info *info)
{
	/*
	 *  Pan (or wrap, depending on the `vmode' field) the display using the
	 *  `xoffset' and `yoffset' fields of the `var' structure.
	 *  If the values don't fit, return -EINVAL.
	 */

	/* ... */
	return -EINVAL;
}

static int av3xx_blank(int blank_mode, const struct fb_info *info)
{
	static int backlight_on = -1;

	switch (blank_mode) {
	case VESA_NO_BLANKING:
		/* printk(KERN_ERR "VESA_NO_BLANKING\n"); */

		/* start oscillation
		 * wait 10ms
		 * cancel standby
		 * turn on LCD power
		 */

		 /* oxygen 77 work to be done */

		if (backlight_on != -1) {
			set_backlight(backlight_on);
		}
		backlight_on = -1;
		break;

	case VESA_VSYNC_SUSPEND:
	case VESA_HSYNC_SUSPEND:
		/* printk(KERN_ERR "VESA_XSYNC_BLANKING\n"); */
		if (backlight_on == -1) {
			backlight_on = get_backlight();
			set_backlight(0);
		}

		/* go to SLP = 1 */
		/* 10101 00001100 */
		break;

	case VESA_POWERDOWN:
		/* printk(KERN_ERR "VESA_POWERDOWN\n"); */
		if (backlight_on == -1) {
			backlight_on = get_backlight();
			set_backlight(0);
		}

		/* got to standby */
		break;

	default:
		/* printk(KERN_ERR "unknown blank value %d\n", blank_mode); */
		return -EINVAL;
	}

	return 0;
}

static void av3xx_set_disp(const void *par, struct display *disp,
			 struct fb_info_gen *info)
{
	/*
	 *  Fill in a pointer with the virtual address of the mapped frame buffer.
	 *  Fill in a pointer to appropriate low level text console operations (and
	 *  optionally a pointer to help data) for the video mode `par' of your
	 *  video hardware. These can be generic software routines, or hardware
	 *  accelerated routines specifically tailored for your hardware.
	 *  If you don't have any appropriate operations, you must fill in a
	 *  pointer to dummy operations, and there will be no text output.
	 */

	disp->screen_base = av3xx_fb;
	disp->dispsw = &fbcon_cfb8;//&fbcon_av3xx;
}


/* ------------ Interfaces to hardware functions ------------ */


struct fbgen_hwswitch av3xx_switch = {
	detect:        av3xx_detect,
	encode_fix:    av3xx_encode_fix,
	decode_var:    av3xx_decode_var,
	encode_var:    av3xx_encode_var,
	get_par:       av3xx_get_par,
	set_par:       av3xx_set_par,
	getcolreg:     av3xx_getcolreg,
	setcolreg:     av3xx_setcolreg,
	pan_display:   av3xx_pan_display,
	blank:         av3xx_blank,
	set_disp:      av3xx_set_disp,
};


/* ------------------------------------------------------------------------- */


/*
 *  Frame buffer operations
 */

static int av3xx_fp_open(const struct fb_info *info, int user)
{
	return 0;
}

#define FBIOGET_CONTRAST	_IOR('F', 0x22, int)
#define FBIOPUT_CONTRAST	_IOW('F', 0x23, int)

#define FBIOGET_BACKLIGHT	_IOR('F', 0x24, int)
#define FBIOPUT_BACKLIGHT	_IOW('F', 0x25, int)

#define FBIO_INIT               _IO ('F', 0x26)

/*oxygen77 to be checked */
#define AV3XX_MIN_CONTRAST 0
#define AV3XX_MAX_CONTRAST 0x7f

static int av3xx_fb_ioctl(struct inode *inode, struct file *file, u_int cmd,
	u_long arg, int con, struct fb_info *info)

{
	int val;

	switch (cmd) {
	case FBIOGET_CONTRAST:
		val = get_contrast();
		if (put_user(val, (int *)arg))
			return -EFAULT;
		break;

	case FBIOPUT_CONTRAST:
		val = (int)arg;
		if (val < AV3XX_MIN_CONTRAST || val > AV3XX_MAX_CONTRAST)
			return -EINVAL;
		set_contrast(val);
		break;

	case FBIOGET_BACKLIGHT:
		val = get_backlight();
		if (put_user(val, (int *)arg))
			return -EFAULT;
		break;

	case FBIOPUT_BACKLIGHT:
		val = (int)arg;
		set_backlight(val);
		break;
		
	case FBIO_INIT:
		init_lcd();
		break;

	default:
		return -EINVAL;
	}

	return 0;
}


/*
 *  In most cases the `generic' routines (fbgen_*) should be satisfactory.
 *  However, you're free to fill in your own replacements.
 */

static struct fb_ops av3xxfb_ops = {
	owner:           THIS_MODULE,
	fb_open:         av3xx_fp_open,
	fb_get_fix:      fbgen_get_fix,
	fb_get_var:      fbgen_get_var,
	fb_set_var:      fbgen_set_var,
	fb_get_cmap:     fbgen_get_cmap,
	fb_set_cmap:     fbgen_set_cmap,
	fb_pan_display:  fbgen_pan_display,
	fb_ioctl:        av3xx_fb_ioctl,
};


/* ------------ Hardware Independent Functions ------------ */


/*
 *  Initialization
 */

int __init av3xxfb_init(void)
{
	fb_info.gen.fbhw = &av3xx_switch;

	fb_info.gen.fbhw->detect();

	strcpy(fb_info.gen.info.modename, "Av3xx");

	fb_info.gen.info.changevar = NULL;
	fb_info.gen.info.node = -1;
	fb_info.gen.info.fbops = &av3xxfb_ops;
	fb_info.gen.info.disp = &disp;
	fb_info.gen.info.switch_con = &fbgen_switch;
	fb_info.gen.info.updatevar = &fbgen_update_var;
	fb_info.gen.info.blank = &fbgen_blank;
	fb_info.gen.info.flags = FBINFO_FLAG_DEFAULT;

	/* This should give a reasonable default video mode */
	fbgen_get_var(&disp.var, -1, &fb_info.gen.info);
	fbgen_do_set_var(&disp.var, 1, &fb_info.gen);
	fbgen_set_disp(-1, &fb_info.gen);
	fbgen_install_cmap(0, &fb_info.gen);

	if ( register_framebuffer(&fb_info.gen.info) < 0 ) {
		return -EINVAL;
	}

	init_lcd();

	printk(KERN_INFO "fb%d: %s frame buffer device\n", GET_FB_IDX(fb_info.gen.info.node), fb_info.gen.info.modename);

	/* uncomment this if your driver cannot be unloaded */
	/* MOD_INC_USE_COUNT; */
	return 0;
}


/*
 *  Cleanup
 */

void av3xxfb_cleanup(struct fb_info *info)
{
	/*
	 *  If your driver supports multiple boards, you should unregister and
	 *  clean up all instances.
	 */

	unregister_framebuffer(info);
	/* ... */
}


/*
 *  Setup
 */

int __init av3xxfb_setup(char *options)
{
	/* Parse user speficied options (`video=av3xxfb:') */
	return 0;
}



/* ------------------------------------------------------------------------- */


/*
 *  Modularization
 */

#ifdef MODULE
MODULE_LICENSE("GPL");
int init_module(void)
{
	return av3xxfb_init();
}

void cleanup_module(void)
{
	av3xxfb_cleanup(void);
}
#endif /* MODULE */

