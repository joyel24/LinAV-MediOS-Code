/*
 * $Id$
 *
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *
 *  Sponsored by SuSE
 */

/*
 * Input line discipline attach program
 */

/*
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Ucitelska 1576, Prague 8, 182 00 Czech Republic
 */

#include <linux/serio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


#define av3xx_speed           B9600
#define av3xx_flags           CS8
#define av3xx_type            SERIO_AV3XX

int readchar(int fd, unsigned char *c, int timeout)
{
	struct timeval tv;
	fd_set set;

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	FD_ZERO(&set);
	FD_SET(fd, &set);

	if (!select(fd+1, &set, NULL, NULL, &tv)) return -1;

	read(fd, c, 1);

	return 0;
}



void setline(int fd, int flags, int speed)
{
	struct termios t;

	tcgetattr(fd, &t);

	t.c_cflag = flags | CREAD | HUPCL | CLOCAL;
	t.c_iflag = IGNBRK | IGNPAR;
	t.c_oflag = 0;
	t.c_lflag = 0;
	t.c_cc[VMIN ] = 1;
	t.c_cc[VTIME] = 0;

	cfsetispeed(&t, speed);
	cfsetospeed(&t, speed);

	tcsetattr(fd, TCSANOW, &t);
}



int main(int argc, char **argv)
{
	unsigned long devt;
	int ldisc;
    int fd;
	char c;

	if ((fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		perror("inputattach");
		return 1;
	}
	setline(fd, av3xx_flags, av3xx_speed);

	/* flushing */
	while (!readchar(fd, &c, 100)) /* nothing */;

	ldisc = N_MOUSE;
	if(ioctl(fd, TIOCSETD, &ldisc)) {
		fprintf(stderr, "inputattach: can't set line discipline\n"); 
		return 1;
	}
	devt = SERIO_RS232 | av3xx_type ;

	if(ioctl(fd, SPIOCSTYPE, &devt)) {
		fprintf(stderr, "inputattach: can't set device type\n");
		return 1;
	}
	read(fd, NULL, 0);
	ldisc = 0;
	ioctl(fd, TIOCSETD, &ldisc);
	close(fd);
	return 0;
}
