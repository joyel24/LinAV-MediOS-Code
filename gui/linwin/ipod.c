/*
 * Copyright (C) 2004 Bernard Leach
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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <fcntl.h>
#ifdef IPOD
#include <linux/fb.h>
#endif
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "av3xx_common.h"

#include "ipod.h"

#define FBIOGET_CONTRAST	_IOR('F', 0x22, int)
#define FBIOSET_CONTRAST	_IOW('F', 0x23, int)

#define FBIOGET_BACKLIGHT	_IOR('F', 0x24, int)
#define FBIOSET_BACKLIGHT	_IOW('F', 0x25, int)

#define FB_DEV_NAME		"/dev/fb0"
#define FB_DEVFS_NAME	"/dev/fb/0"

#define IPOD_SETTINGS_FILE	"/mnt/winlin.cfg"

extern int errno;

static int settings_buffer[100];

static int ipod_ioctl(int request, int *arg)
{
#ifdef IPOD
	int fd;

	fd = open(FB_DEV_NAME, O_NONBLOCK);
	if (fd < 0) fd = open(FB_DEVFS_NAME, O_NONBLOCK);
	if (fd < 0) {
		return -1;
	}
	if (ioctl(fd, request, arg) < 0) {
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
#else
	return -1;
#endif
}

int ipod_get_contrast(void)
{
	int contrast;

	if (ipod_ioctl(FBIOGET_CONTRAST, &contrast) < 0) {
		return -1;
	}

	return contrast;
}

int ipod_set_contrast(int contrast)
{
	if (ipod_ioctl(FBIOSET_CONTRAST, (int *) contrast) < 0) {
		return -1;
	}

	return 0;
}

int ipod_get_mouseParam_freq(void)
{
	struct mouseParam param;

   int fd = 0;

   fd=open("/dev/mouse",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/mouse\n");
		return fd;
   }

	if(ioctl(fd,AV_GET_MOUSE_PARAM,&param)<0)
	{
      printf("Error getting mouse params\n");
		return fd;
   }
   close(fd);

	return param.freq;
}

int ipod_get_mouseParam_repeat(void)
{
	struct mouseParam param;

   int fd = 0;

   fd=open("/dev/mouse",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/mouse\n");
		return fd;
   }

	if(ioctl(fd,AV_GET_MOUSE_PARAM,&param)<0)
	{
      printf("Error getting mouse params\n");
		return fd;
   }
   close(fd);

	return param.repeated_press;
}

int ipod_set_mouseParam(int freq, int repeat)
{
   int fd = 0;
	struct mouseParam param;
	param.freq = freq;
	param.repeated_press = repeat;

   fd=open("/dev/mouse",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/mouse\n");
		return fd;
   }

	if(ioctl(fd,AV_SET_MOUSE_PARAM,&param)<0)
	{
      printf("Error setting mouse params\n");
		return fd;
   }
   close(fd);

	return fd;
}

int ipod_get_backlight(void)
{
	int backlight;

	if (ipod_ioctl(FBIOGET_BACKLIGHT, &backlight) < 0) {
		return -1;
	}

	return backlight;
}

int ipod_set_backlight(int backlight)
{
	if (ipod_ioctl(FBIOSET_BACKLIGHT, (int *) backlight) < 0) {
		return -1;
	}

	return 0;
}

int ipod_set_setting(int setting, int value)
{
	if (value <= 0) {
		value = 0;
	}

	settings_buffer[setting] = value + 1;
	switch (setting) {
	case CONTRAST:
		ipod_set_contrast(value);
		break;	
	case BACKLIGHT:
		ipod_set_backlight(value);
		break;
	case KEY_FREQ:
      ipod_set_mouseParam(value, ipod_get_mouseParam_repeat());
		break;
	case KEY_REPEAT:
      ipod_set_mouseParam(ipod_get_mouseParam_freq(), value);
		break;
	}

	return 0;
}

int ipod_get_setting(int setting)
{

	int value;

	value = settings_buffer[setting] - 1;
	if (value <= 0) {
		value = 0;
	}
	return value;
}

int ipod_load_settings(void)
{
   FILE *fp;
	int x;

	if (fp = fopen(IPOD_SETTINGS_FILE, "r"))
	{
	   if (fread(settings_buffer, sizeof(int), 100, fp) != 100)
		{
			printf("Failed to read LinWin settings from %s.(%d)\n", IPOD_SETTINGS_FILE, errno);
		}
		else
		{
			/* loop may seem pointless, but it's not, or is it? */
			for(x = 0; x <= 100 ; x++)
			{
				ipod_set_setting(x, ipod_get_setting(x));
			}

			printf("Read LinWin settings from %s.(%d)\n", IPOD_SETTINGS_FILE, errno);
		}

		fclose(fp);
	}
	else
	{
		printf("Failed to open %s to read settings, using defaults.(%d)\n", IPOD_SETTINGS_FILE, errno);

		for (x = 1 ; x <= 100 ; x++)
		{
			ipod_set_setting(x, 0);
   	}

		// default values if reading fails
		ipod_set_setting(CONTRAST, ipod_get_contrast());
		ipod_set_setting(CLICKER, 1);
		ipod_set_setting(WHEEL_DEBOUNCE, 6);
		ipod_set_setting(ACTION_DEBOUNCE, 400);
		ipod_set_setting(BACKLIGHT_TIMER, 0);
		ipod_set_setting(REPEAT, 1);
		ipod_set_setting(VOLUME, 80);
      ipod_set_setting(KEY_FREQ, 6);
      ipod_set_setting(KEY_REPEAT, 3);
	}
}

int ipod_save_settings(void)
{
	FILE *fp;
   int x;

	if (fp = fopen(IPOD_SETTINGS_FILE, "w"))
	{
		if (fwrite(settings_buffer, sizeof(int), 100, fp) != 100)
		{
			printf("Failed to write LinWin settings to %s.(%d)\n", IPOD_SETTINGS_FILE, errno);
		}
		else
		{
			printf("Write LinWin settings to %s.(%d)\n", IPOD_SETTINGS_FILE, errno);
		}

		fclose(fp);
	}
	else
	{
		printf("Failed to open %s to save settings(%d).\n", IPOD_SETTINGS_FILE, errno);
	}
}

void ipod_reset_settings(void)
{
	unlink(IPOD_SETTINGS_FILE);

	ipod_load_settings();
	ipod_save_settings();
}

/*
 * 0: screen on
 * 1,2: screen off
 * 3: screen power down
 */
int ipod_set_blank_mode(int blank)
{
#ifdef IPOD
	if (ipod_ioctl(FBIOBLANK, (int *) blank) < 0) {
		return -1;
	}

#endif
	return 0;
}

void ipod_beep(void)
{
#ifdef IPOD
	static int fd = -1; 
	static char buf;

	if (fd == -1 && (fd = open("/dev/ttyS1", O_WRONLY)) == -1
			&& (fd = open("/dev/tts/1", O_WRONLY)) == -1) {
		return;
	}
    
	write(fd, &buf, 1);
#else
	if (isatty(1)) {
		printf("\a");
	}
#endif
}

