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

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "ipod.h"

#include "pz.h"

#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"

#define PATH "/mnt/LinAv"

#define MP3_BUFF_SIZE (1020*100)

void toggle_mp3_repeat(void)
{
    ipod_set_setting(REPEAT, !ipod_get_setting(REPEAT));
}

void set_volume(void)
{
	new_slider_widget(VOLUME, "Volume", 0, 100);
}

int is_mp3_type(char *extension)
{
	return strcmp(extension, ".mp3") == 0;
}


void new_mp3_window(char *filename)
{
	char tmp[1000];
	char vol[5];
	char rep[5];
   int repeat = 0;

	pid_t pid;

   int volume = 50;
	GrClose();

   volume = ipod_get_setting(VOLUME);
   repeat = ipod_get_setting(REPEAT);

	pid = vfork();
	if (pid == 0) {
    	sprintf(tmp, "%s/play",PATH);
    	sprintf(vol, "%d",volume);
    	sprintf(rep, "%d",repeat);
		execl(tmp, "play", filename, vol,rep);
		fprintf(stderr, "exec failed!\n");
		exit(1);
	}
	else {
		if (pid > 0) {
			int status;

			waitpid(pid, &status, 0);
		}
		else {
			fprintf(stderr, "vfork failed %d\n", pid);
		}
	}

	sprintf(tmp, "%s/linwin",PATH);
	execl(tmp, "linwin", "1");
	fprintf(stderr, "Cannot restart linwin!\n");
	exit(1);
}
