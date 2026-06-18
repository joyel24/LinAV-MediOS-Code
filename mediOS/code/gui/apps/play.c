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

int vol, bass = 50, treb = 50, bal = 50, loud = 0;
int cursor_position = 1, fd_mix;
char tmp[1000];

void usage(void)
{
    printf("MP3 player v0.1 by oxygen77\n");
    printf("20/07/2004\n");
    printf("usage: play file\n");
}

GR_GC_ID g_gcWhite;
GR_GC_ID g_gcBlack;
GR_GC_ID g_gcBlue;
GR_GC_ID g_gcRed;
GR_WINDOW_ID g_main;
// arg1 = file; arg2 = volume

void draw_text(void)
{
    sprintf(tmp,"Vol: %03d",vol);
    if(cursor_position == 1)
        GrText(g_main, g_gcRed, 10, 100, tmp, strlen(tmp), GR_TFASCII);
    else
        GrText(g_main, g_gcWhite, 10, 100, tmp, strlen(tmp), GR_TFASCII);

    sprintf(tmp,"Bass: %03d",bass);
    if(cursor_position == 2)
        GrText(g_main, g_gcRed, 10, 115, tmp, strlen(tmp), GR_TFASCII);
    else
        GrText(g_main, g_gcWhite, 10, 115, tmp, strlen(tmp), GR_TFASCII);

    sprintf(tmp,"Treble: %03d",treb);
    if(cursor_position == 3)
        GrText(g_main, g_gcRed, 10, 130, tmp, strlen(tmp), GR_TFASCII);
    else
        GrText(g_main, g_gcWhite, 10, 130, tmp, strlen(tmp), GR_TFASCII);

    sprintf(tmp,"Balance: %03d",bal);
    if(cursor_position == 4)
        GrText(g_main, g_gcRed, 10, 145, tmp, strlen(tmp), GR_TFASCII);
    else
        GrText(g_main, g_gcWhite, 10, 145, tmp, strlen(tmp), GR_TFASCII);

    sprintf(tmp,"Loudness: %03d",loud);
    if(cursor_position == 5)
        GrText(g_main, g_gcRed, 10, 160, tmp, strlen(tmp), GR_TFASCII);
    else
        GrText(g_main, g_gcWhite, 10, 160, tmp, strlen(tmp), GR_TFASCII);
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
    int wait,end;
    int fd_dsp,fd_file;
    

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

    GR_WM_PROPERTIES wmprops;
    GR_EVENT event;

    

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
    end=0;
    frame_cnt=0;
    
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
    g_gcRed = GrNewGC();

    GrSetGCForegroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);
    GrSetGCBackgroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);

    GrSetGCForegroundPixelVal(g_gcWhite, AV3XX_COLOR_WHITE);
    GrSetGCBackgroundPixelVal(g_gcWhite, AV3XX_COLOR_BLACK);

    GrSetGCForegroundPixelVal(g_gcBlue, AV3XX_COLOR_BLUE2);
    GrSetGCBackgroundPixelVal(g_gcBlue, AV3XX_COLOR_BLACK);

    GrSetGCForegroundPixelVal(g_gcRed, AV3XX_COLOR_RED);
    GrSetGCBackgroundPixelVal(g_gcRed, AV3XX_COLOR_BLACK);

    GrMapWindow(g_main);
    GrClearWindow(g_main,0);

    // black out the display
    GrFillRect(g_main, g_gcBlack, 0, 0, 320, 240);

    // "Now Playing: X.mp3"
    sprintf(tmp,"Now Playing: %s",filename);
    GrText(g_main, g_gcWhite, 10, 15, tmp, strlen(tmp), GR_TFASCII);

    draw_text();
    apply_settings();
    
	if(ioctl(fd_dsp,AV_DSP_START_MP3,NULL)<0)
	{
		printf("Error starting\n");
		return -1;
	}

    while(!data.finished)
    {
        


        while(GrCheckNextEvent(&event),event.type!=GR_EVENT_TYPE_NONE)
        {
            if(event.type == GR_EVENT_TYPE_KEY_DOWN)
            {
                switch (event.keystroke.ch)
                {
                    case 'u': // cursor up
                        if(cursor_position > 1)
                            cursor_position--;
                        else
                            cursor_position = 5;
                        draw_text();
                        break;

                    case 'd': // cursor down
                        if(cursor_position < 5)
                            cursor_position++;
                        else
                            cursor_position = 1;
                        draw_text();
                        break;

                    case 'r': // adjust up
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

                    case 'l': // adjust down
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
		    case 'o': // pause/resume
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
		    case 'f': // pause/resume
		    	if(ioctl(fd_dsp,AV_DSP_STOP_MP3,NULL)<0)
			{
				printf("error stoping\n");
				return -1;
			}
			goto end;
		   	break;
                }
            }
        }

        // draw the peak meters
        if(ioctl(fd_dsp,AV_DSP_OUT_PEAK_REAL,&av_p)<0)
        {
            printf("unable to get out peak\n");
        }
        else
        {
            av_p.left=(av_p.left*200)/0x7FFF;
            av_p.right=(av_p.right*200)/0x7FFF;

            // left meter
            GrFillRect(g_main, g_gcBlack, 10+av_p.left, 210, (320-av_p.left)+10, 14);
            GrFillRect(g_main, g_gcBlue, 10, 210, av_p.left, 14);

            // right meter
            GrFillRect(g_main, g_gcBlack, 10+av_p.right, 225, (320-av_p.right)+10, 14);
            GrFillRect(g_main, g_gcBlue, 10, 225, av_p.right, 14);
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
            GrText(g_main, g_gcWhite, 10, 40, tmp, strlen(tmp), GR_TFASCII);
        }
        last_time_cnt = time_cnt;

        // make sure the settings have been applied
        if(settings_applied == 0)
        {
            apply_settings();
            settings_applied = 1;
        }
    }
    
 end:   
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

//    return 0;
}
