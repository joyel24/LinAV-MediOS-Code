#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"


#define MP3_BUFF_SIZE (1020*100)

void usage(void)
{
	printf("MP3 player v0.1 by oxygen77\n");
	printf("20/07/2004\n");
	printf("usage: play file\n");
}

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

		if(ioctl(fd_mix,AV_SET_MIX_VOLUME,&vol)<0)
		{
			printf("unable to set vol\n");
			break;
		}
		
		wait++;
		if(wait>50)
		{
			wait=0;
			if(ioctl(fd_dsp,AV_DSP_FRAME_CNT,&frame_cnt)<0)
			{
				printf("unable to get frame cnt\n");
				break;
			}
			if(ioctl(fd_dsp,AV_DSP_OUT_PEAK,&av_p)<0)
			{
				printf("unable to get out peak\n");
				break;
			}
			
			
			
			//printf("frame = %d L=%d R=%d\n",frame_cnt,av_p.left,av_p.right);
		}


	}

	
	
	close(fd_dsp);
	close(fd_mix);
	close(fd_file);
	free(buff0);
	free(buff1);

	printf("I'm out \n");
	
//	exit(0);

	return 0;
}
