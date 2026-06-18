/*
 * Microwindows mouse driver for av3xx
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#define _LINUX_TYPES_H
#include <linux/fb.h>
#include "av3xx_common.h"


/*****************/

#include "device.h"

/* file descriptor  */
static int pd_fd;


static int PD_Open(MOUSEDEVICE *pmd)
{	 
	pd_fd = open("/dev/mouse", O_NONBLOCK);
	if (pd_fd < 0) {
		fprintf(stderr, "Error %d opening av3xx mouse device\n", -pd_fd);
		return -1;
	}

	//GdHideCursor(&scrdev);
	return pd_fd;
}

static void PD_Close(void)
{
 	if (pd_fd > 0)
		close(pd_fd);
	pd_fd = 0;
}

static int PD_GetButtonInfo(void)
{
 	/* get "mouse" buttons supported */
	return 0;//MWBUTTON_L;
}

static void PD_GetDefaultAccel(int *pscale,int *pthresh)
{
	/*
	 * Get default mouse acceleration settings
	 */
	*pscale = 3;
	*pthresh = 5;
}

static int PD_Read(MWCOORD *px, MWCOORD *py, MWCOORD *pz, int *pb)
{
	/* read a data point */
        struct av3xx_pos data;
	
	if(ioctl(pd_fd,AV_GET_MOUSE_IOC,&data)<0)
	{
		return -1;
	}

	*px = (MWCOORD)data.x;
	*py = (MWCOORD)data.y;

	*pb = 0;
	*pz = 0;

	#if 0
	if((*px == -1 || *py == -1) && *pb >= 0)
		return 3;			/* only have button data */
	if((*px == -1 || *py == -1) && *pb < 0)
		return 0;			/* don't have any data   */
	return 2;				/* have full set of data */
	#endif
	return 2;
}

MOUSEDEVICE mousedev = {
	PD_Open,
	PD_Close,
	PD_GetButtonInfo,
	PD_GetDefaultAccel,
	PD_Read,
	NULL
};