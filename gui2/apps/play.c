#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"

#include "graphics.h"
#include "events.h"

#define MP3_BUFF_SIZE (1020*1000)

#define MAX(a,b)      (a=a<b?a:b)   // a=max(a,b)

needFont(std6x9);
needFont(shadow);

int vol=70, bass = 50, treb = 50, bal = 50, loud = 0;
int cursor_position = 1, fd_mix;
char tmp[1000];

void usage(void)
{
    printf("MP3 player v0.1 by oxygen77\n");
    printf("20/07/2004\n");
    printf("usage: play file\n");
}

GC_ID g_gcWhite;
GC_ID g_gcBlack;
GC_ID g_gcBlue;
GC_ID g_gcRed;

GC_ID g_gcWhite2;
GC_ID g_gcBlack2;
GC_ID g_gcRed2;

// arg1 = file; arg2 = volume

void draw_text(void)
{
    sprintf(tmp,"Vol: %03d",vol);
    if(cursor_position == 1)
    	putS(USE_GC,USE_GC,10, 100, tmp, g_gcRed2);        
    else
    	putS(USE_GC,USE_GC,10, 100, tmp, g_gcWhite2);

    sprintf(tmp,"Bass: %03d",bass);
    if(cursor_position == 2)
        putS(USE_GC,USE_GC,10, 115, tmp, g_gcRed2);        
    else
    	putS(USE_GC,USE_GC,10, 115, tmp, g_gcWhite2);

    sprintf(tmp,"Treble: %03d",treb);
    if(cursor_position == 3)
        putS(USE_GC,USE_GC,10, 130, tmp, g_gcRed2);        
    else
    	putS(USE_GC,USE_GC,10, 130, tmp, g_gcWhite2);

    sprintf(tmp,"Balance: %03d",bal);
    if(cursor_position == 4)
        putS(USE_GC,USE_GC,10, 145, tmp, g_gcRed2);        
    else
    	putS(USE_GC,USE_GC,10, 145, tmp, g_gcWhite2);

    sprintf(tmp,"Loudness: %03d",loud);
    if(cursor_position == 5)
        putS(USE_GC,USE_GC,10, 160, tmp, g_gcRed2);        
    else
    	putS(USE_GC,USE_GC,10, 160, tmp, g_gcWhite2);
}

void apply_settings(void)
{
    // apply volume
    if(ioctl(fd_mix,AV_SET_MIX_VOLUME,&vol)<0)
        printf("unable to set vol\n");

    // apply bass
    if(ioctl(fd_mix,AV_SET_MIX_BASS,&bass)<0)
        printf("unable to set bass\n");

    // apply treble
    if(ioctl(fd_mix,AV_SET_MIX_TREBLE,&treb)<0)
        printf("unable to set treble\n");

    // apply balance
    if(ioctl(fd_mix,AV_SET_MIX_BALANCE,&bal)<0)
        printf("unable to set balance\n");

    // apply loudness
    if(ioctl(fd_mix,AV_SET_MIX_LOUDNESS,&loud)<0)
        printf("unable to set loudness\n");
}

int main(int argc,char * * argv)
{
    int i, cnt1;
    int wait;
    int fd_dsp,fd_file;
    int old_l=0,old_r=0;
    

    char defFilename[]="/mnt/file.mp3";
    char * filename;
    int frame_cnt; // total frames so far
    struct mp3_play data;
    struct av_peak av_p; // used for peakmeters
    char tmp[100]; // used to print text
    int seconds=0, minutes=0,hours=0; // elapsed time
    int time_cnt, last_time_cnt; // elapsed time in seconds
    int settings_applied = 0; // "1" if settings have been applied
    int count = 0;
    int pause=0;
    int evt;
    int mode=0;


    
    if(argc<3)
    {
       filename=defFilename;
    }
    else
    {
        filename=argv[1];
        vol=atoi(argv[2]);
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

    data.size=MP3_BUFF_SIZE;
    data.filename=filename;
    data.pos=0;  
    data.finished=0;

    if(ioctl(fd_dsp,AV_DSP_INI_MP3,&data)<0)
    {
        printf("Error sending data struct\n");
        return -1;
    }

    printf("Ready to play\n");

    
    wait=0;
    frame_cnt=0;
    
    if(ini_graphics()<0)
    {
    	printf("Cannot open graphics\n");
	exit(1);
    }
    
    g_gcWhite = createGC(BMAP1);
    g_gcBlack = createGC(BMAP1);
    g_gcBlue = createGC(BMAP1);
    g_gcRed = createGC(BMAP1);
    
    g_gcWhite2 = createGC(BMAP2);
    g_gcBlack2 = createGC(BMAP2);
    g_gcRed2 = createGC(BMAP2);

    g_gcWhite->fg=16;
    g_gcWhite->bg=0;
    g_gcWhite->font=std6x9;
    
    g_gcBlack->fg=0;
    g_gcBlack->bg=1;
    g_gcBlack->font=std6x9;
    
    g_gcBlue->fg=2;
    g_gcBlue->bg=2;
    g_gcBlue->font=std6x9;
    
    g_gcRed->fg=5;
    g_gcRed->bg=0;
    g_gcRed->font=std6x9;
    
    g_gcWhite2->fg=16;
    g_gcWhite2->bg=0;
    g_gcWhite2->font=std6x9;
    
    g_gcBlack2->fg=0;
    g_gcBlack2->bg=0;
    g_gcBlack2->font=std6x9;
    
    g_gcRed2->fg=5;
    g_gcRed2->bg=0;
    g_gcRed2->font=std6x9;
    
    hidePlane(BMAP2);
    
    fillRect(USE_GC,0, 0 , 320, 240, g_gcBlack); //clearScr();   
    fillRect(USE_GC,0, 0 , 320, 240, g_gcBlack2); //clearScr();   

    // "Now Playing: X.mp3"
    sprintf(tmp,"Now Playing: %s",filename);;
    putS(USE_GC,USE_GC,10, 15, tmp, g_gcWhite);
    
    draw_text();
    apply_settings();
    
	if(ioctl(fd_dsp,AV_DSP_START_MP3,NULL)<0)
	{
		printf("Error starting\n");
		return -1;
	}
	

    while(!data.finished)
    {
        while((evt=nxtEvent())>0)
        {
		if(mode)
		{
                switch (evt)
                {
                    case BTN_UP: // cursor up
                        if(cursor_position > 1)
                            cursor_position--;
                        else
                            cursor_position = 5;
                        draw_text();
                        break;

                    case BTN_DOWN: // cursor down
                        if(cursor_position < 5)
                            cursor_position++;
                        else
                            cursor_position = 1;
                        draw_text();
                        break;

                    case BTN_RIGHT: // adjust up
                        switch(cursor_position)
                        {
                            case 1: if(vol < 100) vol++; break;
                            case 2: if(bass < 100) bass+=5; break;
                            case 3: if(treb < 100) treb+=5; break;
                            case 4: if(bal < 100) bal++; break;
                            case 5: if(loud < 100) loud+=5; break;
                        }
                        draw_text();
                        apply_settings();
                        break;

                    case BTN_LEFT: // adjust down
                        switch(cursor_position)
                        {
                            case 1: if(vol > 0) vol--; break;
                            case 2: if(bass > 0) bass-=5; break;
                            case 3: if(treb > 0) treb-=5; break;
                            case 4: if(bal > 0) bal--; break;
                            case 5: if(loud > 0) loud-=5; break;
                        }
                        draw_text();
                        apply_settings();
                        break;
		case BTN_F1: //switch graphical state
		    	mode=0;
			hidePlane(BMAP2);
			showPlane(BMAP1);
			break;
		}
		}
		else
		{
		switch(evt) {		
		    case BTN_ON: // pause/resume
		    	if(pause)
			{
				if(ioctl(fd_dsp,AV_DSP_START_MP3,NULL)<0)
				{
					printf("Error resuming\n");
					return -1;
				}				
				pause=0;
			}
			else
			{
				if(ioctl(fd_dsp,AV_DSP_PAUSE_MP3,NULL)<0)
				{
					printf("Error pausing\n");
					return -1;
				}
				pause=1;
			}
			
		   	break;
		    case BTN_OFF: // pause/resume
		    	if(ioctl(fd_dsp,AV_DSP_STOP_MP3,NULL)<0)
			{
				printf("error stoping\n");
				return -1;
			}
			goto end;
		   	break;
		    case BTN_F1: //switch graphical state
		    	mode=1;
			draw_text();
			hidePlane(BMAP1);
			showPlane(BMAP2);
			break;
                }
		}
            }
        

	/*wait++;
	if(wait>5000)
	{
		wait=0;*/
        // draw the peak meters
        if(ioctl(fd_dsp,AV_DSP_OUT_PEAK_REAL,&av_p)<0)
        {
            printf("unable to get out peak\n");
        }
        else
        {
            av_p.left=(av_p.left*200)/0x7FFF;
            av_p.right=(av_p.right*200)/0x7FFF;
	
	    MAX(av_p.left,310);
	    MAX(av_p.right,310);
            // left meter
	    if(old_l!=av_p.left)
	    {
		if(old_l>av_p.left)
			fillRect(USE_GC,10+av_p.left, 210, old_l-av_p.left, 14, g_gcBlack);
		else
			fillRect(USE_GC,10+old_l, 210, av_p.left-old_l, 14, g_gcBlue);
		old_l=av_p.left;
	    }
            // right meter
	    if(old_r!=av_p.right)
	    {
		if(old_r>av_p.right)
			fillRect(USE_GC,10+av_p.right, 226, old_r-av_p.right, 14, g_gcBlack);
		else
			fillRect(USE_GC,10+old_r, 226, av_p.right-old_r, 14, g_gcBlue);
		old_r=av_p.right;
	    }
	    
	    
	    
        }

        // draw the elapsed time display
        // 40 frames = 1 second
        if(count == 40)
        {
            ioctl(fd_dsp,AV_DSP_FRAME_CNT,&frame_cnt);
            count = 0;
        }
        count++;

        time_cnt = (frame_cnt/40);

        // only update this when needed, or it flickers
        if(last_time_cnt != time_cnt)
        {
            sprintf(tmp,"Elapsed Time: %02d:%02d:%02d",time_cnt/3600,time_cnt%3600/60,time_cnt%60);
	    putS(USE_GC,USE_GC,10, 40, tmp, g_gcWhite); 
        }
        last_time_cnt = time_cnt;

        // make sure the settings have been applied
        if(settings_applied == 0)
        {
            apply_settings();
            settings_applied = 1;
        }
	//}
    }
    
 end:   
    
 destroyGC(g_gcWhite);
 destroyGC(g_gcBlack);
 destroyGC(g_gcBlue);
 destroyGC(g_gcRed);
 

    close(fd_dsp);
    close(fd_mix);
    printf("mixer closed\n");
    
    close_graphics();

    printf("I'm out \n");

    exit(0);

//    return 0;
}
