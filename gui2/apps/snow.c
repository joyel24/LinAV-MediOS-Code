
#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"


#define NUM_PARTICLES 320

#define LCD_HOEHE   240
#define LCD_BREITE  320

#define AV3XX_COLOR_BLACK 0
#define AV3XX_COLOR_WHITE 1

static short particles[NUM_PARTICLES][2];

int g_cntSnow = 0;
int g_cntPartSnow = 0;

GC_ID g_gcWhite;
GC_ID g_gcBlack;


void ClearScreen();
int GetTime();
void delay(unsigned int time);
void printNr(char* buffer, int nr, int withSign);

int particle_exists(int particle);
int create_particle(void);
void snow_move(void);
void snow_init(void);

int main()
{
	ini_graphics();
    
    g_gcWhite = createGC(BMAP1);
    g_gcBlack = createGC(BMAP1);

    g_gcWhite->fg=16;
    g_gcWhite->bg=1;

    
    g_gcBlack->fg=1;
    g_gcBlack->bg=1;


	//GrHideCursor();
	//delay(100000);
	//GrShowCursor();
   snow_init();

	while (1)
	{
		

      snow_move();

  		
	}
}

void ClearScreen()
{
	int i = 0;
	int xPos = 0;
	
	fillRect(USE_GC,0, 0 , 320, 240, g_gcBlack); //clearScr();   

	
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

int particle_exists(int particle)
{
    if (particles[particle][0]>=0 && particles[particle][1]>=0 &&
        particles[particle][0]<LCD_BREITE && particles[particle][1]<LCD_HOEHE-g_cntPartSnow-1)
        return 1;
    else
        return 0;
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
		  drawPixel(USE_GC,particles[i][0],particles[i][1], g_gcBlack);  // Pixel loeschen
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
               drawPixel(USE_GC,particles[i][0],particles[i][1], g_gcWhite); // Pixel setzen
            }

            if(g_cntSnow > 300)
				{
				fillRect(USE_GC,0, LCD_HOEHE-g_cntPartSnow, LCD_BREITE, g_cntPartSnow, g_gcWhite);
  
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
