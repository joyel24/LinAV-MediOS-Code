/*
* play.c
*
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "font.h"
#include "colordef.h"

#include "mp3_player.h"

struct client_operations * cops;

/***********************
 * Miscellaneous DEFINEs
 **********************/
#define MP3_BUFF_SIZE (1020*2000)
#define MWINCLUDECOLORS


/******************************
 * Color order for LCD printing
 *****************************/
const char *colortext[] = {
"White     ",
"Gray      ",
"Black     ",
"Blue      ",
"Dark Blue ",
"Red       ",
"Green     ",
"Dark Green",
"Yellow    ",
"Orange    "
};

/*******************************
 * And their corresponing colors
 ******************************/
const char colortable[] = {
COLOR_WHITE,
COLOR_GRAY,
COLOR_BLACK,
COLOR_BLUE,
COLOR_DARK_BLUE,
COLOR_RED,
COLOR_GREEN,
COLOR_DARK_GREEN,
COLOR_YELLOW,
COLOR_ORANGE
};

/*************************
 * Ints for PLAYBACK/SOUND
 ************************/
/* sound settings */
int vol=70, bass = 50, treb = 50, bal = 50, loud = 0;
int oldvol;                  /* used for pause/resume */

/******************
 * Ints for WINDOWS
 *****************/
int window = MAIN_WIN;                   /* "1" = main, "2" = settings, "3" = sound */

/*********************
 * Miscellaneous CHARs
 ********************/
char * filename;                    /* current file location */

FILE * fd;
int file_size;
pthread_t read_thread;
//pthread_t draw_thread;
int stopThread=0;

/*******************
 * Other
 ******************/
struct mp3_play data; /* mp3 data */
bool v1first = false;



void mainLoop(void)
{
    //int size,frame;
    //mp3_read_more();
    //frame=cops->readFrame();
    //printf("frame=%d time=%d\n",frame,(frame*26)/1000);
    //printf("OUT: %d IN: %d\n",data.buffer_read,data.buffer_write);
    //drawPeak();
}

struct id3tag fTag;

int main(int argc, char * * argv)
{
    
    REGISTER(cops,eventHandler,0);
    
#ifdef AV_SCREEN
   if(argc<2)
       return 0; 
    else
    {
        filename=argv[1];
    }
    
    fd=fopen(filename,"ro");
    if(fd<0)
    {
            fprintf(stderr,"Can't open file %s\n",filename);
            return -1;
    }
    
    fseek(fd,0,SEEK_END);
    file_size = ftell(fd);
    fseek(fd,0,SEEK_SET);
    
    printf ("File opened size=%d\n",file_size);
    
#endif
    
    /* getting id3info */
    
    if(mp3info(&(fTag.id3), filename, v1first))
    {
            printf("Bad mp3\n");
    }
    else
        debug_info(filename,&(fTag.id3));
   // fseek(fd,fTag.id3.first_frame_offset,SEEK_SET);
    
    data.buffer_len=MP3_BUFF_SIZE;
    data.pos=0;
    data.finished=0;
    data.buffer_read=0;
    data.buffer_write=0;
    data.endOfFile=0;
    data.freqPeakDraw=10;
    data.peakDraw=drawPeak;
    
    data.buffer=(char*)malloc(data.buffer_len);
    if(!data.buffer)
    {
            fprintf(stderr,"Can't allocate buff (size=%d)\n",data.buffer_len);
            fclose(fd);
            return -1;
    }

    printf("buffer created %d\n",data.buffer_len);    
 

    /* initialize the graphics and clear the lcd */
    cops->hideSBar();
    cops->disableMenu();
    cops->clearScreen(COLOR_BLACK); /* clear */

    /* set standard font */
    cops->setFont(STD6X9);
    refreshScreen(MAIN_WIN);
    
    /* Launching read thread */
    /*if (pthread_create(&read_thread, NULL, mp3_read_more, 0) != 0)
    {
        printf("Error, can't create read thread\n");
        fclose(fd);
        free(data.buffer);
        return 0;         
    }*/
    mp3_read_more();
    
    /* initialize mp3 playback */
    if(!cops->ini_mp3_playback(&data))  
    	return 0;
    printf("ini of mp3 driver done\n");  
    
    /* Launching draw thread */
    /*if (pthread_create(&draw_thread, NULL, drawPeak, 0) != 0)
    {
        printf("Error, can't create draw thread\n");
        fclose(fd);
        free(data.buffer);
        return 0;         
    }*/
    
    apply_settings();  
    /* start mp3 */
    cops->start_playback();
    
    PACK(cops,NULL);
    fprintf(stderr,"I've been released\n");
    fclose(fd);
    free(data.buffer);
    fprintf(stderr,"file closed and buffer freed\n");
    return 1;
}

char *secs2str(int ms)
{
    static char buffer[32];
    int secs = ms/1000;
    ms %= 1000;
    snprintf(buffer, sizeof(buffer), "%d:%02d.%d", secs/60, secs%60, ms/100);
    return buffer;
}

void debug_info(char * filename,struct mp3entry * mp3)
{
    printf("****** File: %s\n"
               "      Title: %s\n"
               "     Artist: %s\n"
               "      Album: %s\n"
               "      Genre: %s (%d) \n" 
               "   Composer: %s\n"        
               "       Year: %s (%d)\n"
               "      Track: %s (%d)\n"        
               "     Length: %s / %d s\n"
               "    Bitrate: %d\n"
               "  Frequency: %d\n",
               filename,
               mp3->title?mp3->title:"<blank>",
               mp3->artist?mp3->artist:"<blank>",
               mp3->album?mp3->album:"<blank>",
               mp3->genre_string?mp3->genre_string:"<blank>",
                    mp3->genre,
               mp3->composer?mp3->composer:"<blank>",
               mp3->year_string?mp3->year_string:"<blank>",
                    mp3->year,
               mp3->track_string?mp3->track_string:"<blank>",
                    mp3->tracknum,
               secs2str(mp3->length),
               mp3->length/1000,
               mp3->bitrate,
               mp3->frequency);
}
