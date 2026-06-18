
#include <stdio.h>
#include <stdlib.h>
#define MWINCLUDECOLORS
#include "nano-X.h"
#include "nxcolors.h"
//#include "av3xx_colordef.h"

#define NUM_PARTICLES 320

#define LCD_HOEHE   240/2
#define LCD_BREITE  320/2

#define AV3XX_COLOR_BLACK 0
#define AV3XX_COLOR_WHITE 1

static short particles[NUM_PARTICLES][2];

int g_cntSnow = 0;
int g_cntPartSnow = 0;

GR_GC_ID g_gcWhite;
GR_GC_ID g_gcBlack;
GR_WINDOW_ID g_main;

void ClearScreen();
int GetTime();
void delay(unsigned int time);
void printNr(char* buffer, int nr, int withSign);

GR_BOOL particle_exists(int particle);
int create_particle(void);
void snow_move(void);
void snow_init(void);

int main()
{
	GR_WM_PROPERTIES wmprops;

	if (GrOpen() < 0) {
		fprintf(stderr, "Cannot open graphics\n");
		exit(1);
	}

	g_main = GrNewWindow(GR_ROOT_WINDOW_ID, 0,0, LCD_BREITE, LCD_HOEHE, 0, GR_COLOR_BLACK, 0);

	if(g_main)
	{
		wmprops.flags = GR_WM_FLAGS_PROPS | GR_WM_FLAGS_TITLE;
		wmprops.props = GR_WM_PROPS_APPWINDOW;
		wmprops.title = "Snow";
		GrSetWMProperties(g_main, &wmprops);
	}

	GrSelectEvents(g_main, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	g_gcWhite = GrNewGC();
	g_gcBlack = GrNewGC();

	GrSetGCForegroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);
	GrSetGCBackgroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);

	GrSetGCForegroundPixelVal(g_gcWhite, AV3XX_COLOR_WHITE);
	GrSetGCBackgroundPixelVal(g_gcWhite, AV3XX_COLOR_BLACK);

	GrMapWindow(g_main);

	//GrHideCursor();
	//delay(100000);
	//GrShowCursor();
   snow_init();

	while (1)
	{
		GR_EVENT event;

      snow_move();

  		while(GrCheckNextEvent(&event),event.type!=GR_EVENT_TYPE_NONE)
		{
			if(event.type == GR_EVENT_TYPE_KEY_DOWN)
			{
				switch (event.keystroke.ch)
				{
					case 'f': // off
						GrDestroyGC(g_gcWhite);
						GrDestroyGC(g_gcBlack);

						GrClose();
						exit(0);
				}
			}
		}
	}
}

void ClearScreen()
{
	int i = 0;
	int xPos = 0;

	GrClearWindow(g_main,0);

   GrFillRect(g_main, g_gcBlack, 0, 0, LCD_BREITE, LCD_HOEHE);
}

int GetTime()
{
   int seconds = 0;
/*
   ourTime = rtcGetTime();

   seconds = ourTime->tm_hour*3600;
   seconds += ourTime->tm_min*60;
   seconds += ourTime->tm_sec;
*/
   return seconds;
}

void delay(unsigned int time) {
    for (;time>0;time--) ;
}

GR_BOOL particle_exists(int particle)
{
    if (particles[particle][0]>=0 && particles[particle][1]>=0 &&
        particles[particle][0]<LCD_BREITE && particles[particle][1]<LCD_HOEHE-g_cntPartSnow-1)
        return GR_TRUE;
    else
        return GR_FALSE;
}

int create_particle(void)
{
    int i;

    for (i=0; i<NUM_PARTICLES; i++) {
        if (!particle_exists(i)) {
            particles[i][0]=(rand()%LCD_BREITE);
            particles[i][1]=0;
            return i;
        }
    }
    return -1;
}

void snow_move(void)
{
    int i;

    if (!(rand()%2))
        create_particle();

    for (i=0; i<NUM_PARTICLES; i++)
	 {
        if (particle_exists(i))
		  {
            GrPoint(g_main, g_gcBlack, particles[i][0],particles[i][1]); // Pixel loeschen
            switch ((rand()%7)) {
                case 0:
                    particles[i][0]++;
                    break;

                case 1:
                    particles[i][0]--;
                    break;

                case 2:
                    break;

                default:
                    particles[i][1]++;

						  if(particles[i][1] > LCD_HOEHE-5-g_cntPartSnow)
						  {
						     g_cntSnow++;
						  }
                    break;
            }
            if (particle_exists(i))
				{
               GrPoint(g_main, g_gcWhite, particles[i][0],particles[i][1]); // Pixel setzen
            }

            if(g_cntSnow > 300)
				{
               GrFillRect(g_main, g_gcWhite, 0, LCD_HOEHE-g_cntPartSnow, LCD_BREITE, LCD_HOEHE);
					g_cntSnow = 0;
					g_cntPartSnow++;
				}
        }
    }
}

void snow_init(void)
{
    int i;

    for (i=0; i<NUM_PARTICLES; i++) {
        particles[i][0]=-1;
        particles[i][1]=-1;
    }

    ClearScreen();
}

void printNr(char* buffer, int nr, int withSign)
{
   int begin = 0;

   if(withSign == 1)
   {
      if(nr < 0)
      {
         nr=nr*(-1);
         buffer[begin++] = '-';
      }
      else
      {
         buffer[begin++] = '+';
      }
   }

   buffer[begin++] = '0' + (nr / 10000) % 10;
   buffer[begin++] = '0' + (nr / 1000) % 10;
   buffer[begin++] = '0' + (nr / 100) % 10;
   buffer[begin++] = '0' + (nr / 10) % 10;
   buffer[begin++] = '0' + (nr / 1) % 10;
   buffer[begin++] = 0;
}
