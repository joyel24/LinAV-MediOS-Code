/*
* driver/char/joystick/av3xx_kbd.c
*
* based on drivers/char/joystick/stowaway.c by Justin Cormack <j.cormack@doc.ic.ac.uk>
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <linux/malloc.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>

MODULE_AUTHOR("THOMAS Christophe <oxygen77@free.fr>");

#define AV3XX_KEY	0x7f
#define AV3XX_RELEASE	0x80

static unsigned char av3xx_keycode[128] = {
KEY_5, KEY_6, KEY_7, KEY_8, KEY_G, KEY_H, KEY_I, KEY_J, 0 /*date,apps*/, 0,
0 /*todo,calc*/,0 /*memo,find*/, 0, 0, 0, 0, KEY_9, KEY_0, KEY_A, KEY_B,
KEY_S, KEY_T, KEY_U, KEY_V, KEY_LEFTCTRL, 0 /*Fn*/, KEY_LEFTSHIFT, KEY_LEFTALT, 0, 0,
0, 0, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_W, KEY_X, KEY_Y, KEY_Z,
KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, 0, 0, 0, 0, KEY_MINUS, KEY_EQUAL,
KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_SLASH, KEY_TAB, KEY_CAPSLOCK, 0 /*cmd*/, 0 /*done,cancel*/, KEY_DELETE, KEY_BACKSPACE, KEY_ENTER,
0, 0, 0, 0, KEY_C, KEY_D, KEY_E, KEY_F, KEY_APOSTROPHE, KEY_GRAVE,
KEY_SPACE, 0 /*space2*/, 0, 0, 0, 0, 0, 0, 0, 0,
KEY_1, KEY_2, KEY_3, KEY_4, KEY_K, KEY_L, KEY_M, KEY_N, KEY_BACKSLASH, KEY_SEMICOLON,
KEY_COMMA, KEY_DOT, 0, 0, 0, 0, 0, 0, 0


/*
  KEY_1, KEY_2, KEY_3, KEY_Z, KEY_4, KEY_5, KEY_6, KEY_7,
  0, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_GRAVE,
  KEY_X, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_SPACE, 
  KEY_CAPSLOCK, KEY_TAB, KEY_LEFTCTRL, 0, 0, 0, 0, 0,
  0, 0, 0, KEY_LEFTALT, 0, 0, 0, 0,
  0, 0, 0, 0, KEY_C, KEY_V, KEY_B, KEY_N,
  KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE, KEY_HOME, KEY_8, KEY_9, KEY_0, KEY_ESC,
  KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_BACKSLASH, KEY_END, KEY_U, KEY_I, KEY_O, KEY_P,
  KEY_APOSTROPHE, KEY_ENTER, KEY_PAGEUP, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON,
  KEY_SLASH, KEY_UP, KEY_PAGEDOWN, 0, KEY_M, KEY_COMMA, KEY_DOT, KEY_INSERT,
  KEY_DELETE, KEY_LEFT, KEY_DOWN, KEY_RIGHT, 0, 0, 0, 0,
  KEY_LEFTSHIFT, KEY_RIGHTSHIFT, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7,
  KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, 0, 0, 0*/
};

static char *av3xx_name = "Av3xx Keyboard";

static int count_av_kbd=0;

struct av3xx_s {
  unsigned char keycode[128];
  struct input_dev dev;
  struct serio *serio;
};

void av3xx_interrupt(struct serio *serio, unsigned char data, unsigned int flags)
{
  struct av3xx_s *av3xx_s = serio->private;

  if (av3xx_s->keycode[data & AV3XX_KEY])
    input_report_key(&av3xx_s->dev, av3xx_s->keycode[data & AV3XX_KEY], !(data & AV3XX_RELEASE));
  else
	printk("%d| unknown code: %x\n",count_av_kbd++,data);
}

void av3xx_connect(struct serio *serio, struct serio_dev *dev)
{
	struct av3xx_s *av3xx_s;
	int i;

	if (serio->type != (SERIO_RS232 | SERIO_AV3XX)) return;

	if (!(av3xx_s = kmalloc(sizeof(struct av3xx_s), GFP_KERNEL))) return;

	memset(av3xx_s, 0, sizeof(struct av3xx_s));
	
	av3xx_s->dev.evbit[0] = BIT(EV_KEY) | BIT(EV_REP);

	av3xx_s->serio = serio;

	av3xx_s->dev.keycode = av3xx_s->keycode;
	av3xx_s->dev.private = av3xx_s;

	serio->private = av3xx_s;

	if (serio_open(serio, dev)) {
		kfree(av3xx_s);
		return;
	}

	memcpy(av3xx_s->keycode, av3xx_keycode, sizeof(av3xx_s->keycode));
	for (i = 0; i < 128; i++)
		set_bit(av3xx_s->keycode[i], av3xx_s->dev.keybit);
	clear_bit(0, av3xx_s->dev.keybit);

	av3xx_s->dev.name = av3xx_name;
	av3xx_s->dev.idbus = BUS_RS232;
	av3xx_s->dev.idvendor = SERIO_AV3XX;
	av3xx_s->dev.idproduct = 0x0001;
	av3xx_s->dev.idversion = 0x0100;

	input_register_device(&av3xx_s->dev);

	printk(KERN_INFO "input%d: %s on serio%d\n", av3xx_s->dev.number, av3xx_name, serio->number);
}

void av3xx_disconnect(struct serio *serio)
{
	struct av3xx_s *av3xx_s = serio->private;
	input_unregister_device(&av3xx_s->dev);
	serio_close(serio);
	kfree(av3xx_s);
}

struct serio_dev av3xx_dev = {
	interrupt:	av3xx_interrupt,
	connect:	av3xx_connect,
	disconnect:	av3xx_disconnect
};

int __init av3xx_init(void)
{
	serio_register_device(&av3xx_dev);
	return 0;
}

void __exit av3xx_exit(void)
{
	serio_unregister_device(&av3xx_dev);
}

module_init(av3xx_init);
module_exit(av3xx_exit);
