
#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "events.h"
#include "avwm.h"


#define NUM_PARTICLES 320

#define LCD_HOEHE   240
#define LCD_BREITE  320

#define BLACK 1
#define WHITE 16

static short particles[NUM_PARTICLES][2];

int g_cntSnow = 0;
int g_cntPartSnow = 0;

void ClearScreen();
int GetTime();
void delay(unsigned int time);
void printNr(char* buffer, int nr, int withSign);

int particle_exists(int particle);
int create_particle(void);
void snow_move(void);
void snow_init(void);

struct client_operations * cops;
int stop,blocked;

int eventHandler(int evt)
{
	switch(evt) {
		case BTN_OFF:
		case EVT_QUIT:
			stop=1;
			break;
		case EVT_SUSPEND:
			blocked=1;
			break;
		case EVT_RESUME:
			blocked=0;
			break;
		}
}

int main(int argc,char * * argv)
{
	int evt;
	REGISTER(cops,eventHandler);	
 stop=0;
 blocked=0;
   snow_init();
PACK(cops);
	while (!stop)
	{
	
	while(blocked) /* wait */;	

      snow_move();

  		
	}
}

void ClearScreen()
{
	int i = 0;
	int xPos = 0;
	
	cops->fillRect(BLACK,0, 0 , 320, 240); //clearScr();   

	
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
		  cops->drawPixel(BLACK,particles[i][0],particles[i][1]);  // Pixel loeschen
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
               cops->drawPixel(WHITE,particles[i][0],particles[i][1]); // Pixel setzen
            }

            if(g_cntSnow > 300)
				{
				cops->fillRect(WHITE,0, LCD_HOEHE-g_cntPartSnow, LCD_BREITE, g_cntPartSnow);
  
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
