#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"
#define MWINCLUDECOLORS
#include "nano-X.h"
#include "nxcolors.h"
#include "av3xx_colordef.h"

#define MP3_BUFF_SIZE (1020*100)

void usage(void)
{
	printf("MP3 player v0.1 by oxygen77\n");
	printf("20/07/2004\n");
	printf("usage: play file\n");
}
GR_GC_ID g_gcWhite;
GR_GC_ID g_gcBlack;
GR_GC_ID g_gcBlue;
GR_WINDOW_ID g_main;
// arg1 = file; arg2 = volume
int main(int argc,char * * argv)
{
	int i,cnt1,cnt2,start,tot;
	int vol,inc,wait,end;
	int source_pos;
	int fd_dsp,fd_file,fd_mix;
	char *buff0,*buff1;
	int size=sizeof(char)*MP3_BUFF_SIZE;
	char defFilename[]="/mnt/file.mp3";
	char * filename;
	int frame_cnt;
	struct mp3_play data;
	struct av_peak av_p;
	char tmp[1000];
	int old_r,old_l;
	
	GR_WM_PROPERTIES wmprops;
	GR_EVENT event;
	
	if (GrOpen() < 0) {
		fprintf(stderr, "Cannot open graphics\n");
		exit(1);
	}

	g_main = GrNewWindow(GR_ROOT_WINDOW_ID, 0,0, 320, 240, 0, GR_COLOR_BLACK, 0);

	if(g_main)
	{
		wmprops.flags = GR_WM_FLAGS_PROPS | GR_WM_FLAGS_TITLE;
		wmprops.props = GR_WM_PROPS_APPWINDOW;
		wmprops.title = "Mp3 play";
		GrSetWMProperties(g_main, &wmprops);
	}

	GrSelectEvents(g_main, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);


	g_gcWhite = GrNewGC();
	g_gcBlack = GrNewGC();
	g_gcBlue = GrNewGC();

	GrSetGCForegroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);
	GrSetGCBackgroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);

	GrSetGCForegroundPixelVal(g_gcWhite, AV3XX_COLOR_WHITE);
	GrSetGCBackgroundPixelVal(g_gcWhite, AV3XX_COLOR_BLACK);
	
	GrSetGCForegroundPixelVal(g_gcBlue, AV3XX_COLOR_BLUE2);
	GrSetGCBackgroundPixelVal(g_gcBlue, AV3XX_COLOR_BLACK);

	

	
	printf("size=%d %d\n",size,MP3_BUFF_SIZE);

	buff0=(char*)malloc(size);
	buff1=(char*)malloc(size);

	printf("argc: %d\n", argc);

	if(argc<3)
	{
	   return 0; // Quit
	}
	else
   {
		filename=argv[1];
		vol=atoi(argv[2]);
	}

	if(!buff0 || !buff1)
	{
		printf("Error allocating buffer\n");
		return -1;
	}

	printf("In soundTest\n");

	fd_dsp=open("/dev/dsp",O_WRONLY);
	if (fd_dsp < 0) {
		printf("Can't open /dev/dsp\n");
		return -1;
	}

	fd_mix=open("/dev/mixer",O_WRONLY);
	if (fd_mix < 0) {
		printf("Can't open /dev/mixer\n");
		return -1;
	}

	fd_file=open(filename,O_RDONLY);
	if (fd_file < 0 ) {
		printf("Can't open file %s\n",filename);
		return -1;
	}

	data.size=MP3_BUFF_SIZE;
	data.cur=buff0;
	data.tmp=buff1;
	data.needData=1;
	data.decRunning=0;
	data.finished=0;
	data.nxt=NULL;

	if(ioctl(fd_dsp,AV_DSP_INI_MP3,&data)<0)
	{
		printf("Error sending data struct\n");
		return -1;
	}

	printf("data send successfuly\n");

	printf("volume: %d\n", vol);

//	vol=0;
//	inc=1;
	wait=0;
	end=0;
	frame_cnt=0;
	old_r=0;
	old_l=0;
	
	GrMapWindow(g_main);
	GrClearWindow(g_main,0);

   	GrFillRect(g_main, g_gcBlack, 0, 0, 320, 240);
	sprintf(tmp,"Playing:  %s",filename);
	GrText(g_main, g_gcWhite, 10, 15, tmp, strlen(tmp), GR_TFASCII);
	sprintf(tmp,"Vol:  %d",vol);
	GrText(g_main, g_gcWhite, 10, 40, tmp, strlen(tmp), GR_TFASCII);

	while(!data.finished)
	{
		if(data.needData & !end)
		{
			cnt1=read(fd_file,data.tmp,MP3_BUFF_SIZE);
			if(cnt1<=0)
			{	
				printf("End of file\n");
				if(ioctl(fd_dsp,AV_DSP_STOP_MP3,NULL)<0)
				{
					printf("Error Stoping MP3 playback\n");
					return -1;
				}
				end=1;
			}
			
			
			if(!data.decRunning)
			{
				data.nxt=data.tmp;
				data.tmp=data.cur;
				data.cur=data.nxt;
				data.nxt=NULL;
				data.decRunning=1;
				if(ioctl(fd_dsp,AV_DSP_START_MP3,NULL)<0)
				{
					printf("Error sync\n");
					return -1;
				}
			}
			else
			{
				data.nxt=data.tmp;
				data.needData=0;
			}
		}

				
		
		
		while(GrCheckNextEvent(&event),event.type!=GR_EVENT_TYPE_NONE)
		{
			if(event.type == GR_EVENT_TYPE_KEY_DOWN)
			{
				switch (event.keystroke.ch)
				{
					/*case 'f': // off
						GrDestroyGC(g_gcWhite);
						GrDestroyGC(g_gcBlack);

						GrClose();
						goto out;*/
					case 'u': //up volume
						vol++;
						if(vol>100)
							vol=100;
						if(ioctl(fd_mix,AV_SET_MIX_VOLUME,&vol)<0)
						{
							printf("unable to set vol\n");
							break;
						}
						// displaying volume 
						sprintf(tmp,"Vol:  %d",vol);
						GrText(g_main, g_gcWhite, 10, 40, tmp, strlen(tmp), GR_TFASCII);
						break;
					case 'd': //down volume
						vol--;
						if(vol<0)
							vol=0;
						if(ioctl(fd_mix,AV_SET_MIX_VOLUME,&vol)<0)
						{
							printf("unable to set vol\n");
							break;
						}
					sprintf(tmp,"Vol:  %d",vol);
					GrText(g_main, g_gcWhite, 10, 40, tmp, strlen(tmp), GR_TFASCII);
					break;
				}
			}
		}
		
		if(ioctl(fd_dsp,AV_DSP_OUT_PEAK_REAL,&av_p)<0)
		{
			printf("unable to get out peak\n");
			
		}
		else
		{
			av_p.left=(av_p.left*200)/0x7FFF;
			av_p.right=(av_p.right*200)/0x7FFF;
			if(old_l>av_p.left)
				GrFillRect(g_main, g_gcBlack, 10+av_p.left, 60, old_l-av_p.left, 10);
			else
				GrFillRect(g_main, g_gcBlue, 10+old_l, 60, av_p.left-old_l, 10);
				
			if(old_r>av_p.right)
				GrFillRect(g_main, g_gcBlack, 10+av_p.right, 70, old_r-av_p.right, 10);
			else
				GrFillRect(g_main, g_gcBlue, 10+old_r, 70, av_p.right-old_r, 10);
			old_l=av_p.left;
			old_r=av_p.right;
			
		}
		
		if(ioctl(fd_dsp,AV_DSP_FRAME_CNT,&frame_cnt)<0)
		{
			printf("unable to get frame cnt\n");
			break;
		}
		else
		{
			sprintf(tmp,"Frame:  %d",frame_cnt);
			GrText(g_main, g_gcWhite, 60, 40, tmp, strlen(tmp), GR_TFASCII);
		}
		
		


	}
GrDestroyGC(g_gcWhite);
	GrDestroyGC(g_gcBlack);

	GrClose();

	close(fd_dsp);
	close(fd_mix);
	printf("mixer closed\n");
	close(fd_file);
	//printf("file closed\n");
	//free(buff0);
	//printf("buff0 free\n");
	//free(buff1);

	printf("I'm out \n");
	
	exit(0);

//	return 0;
}
