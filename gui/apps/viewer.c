
#include <stdio.h>
#include <stdlib.h>
#define MWINCLUDECOLORS
#include "nano-X.h"
#include "nxcolors.h"

#define LCD_HOEHE   240
#define LCD_BREITE  320

GR_GC_ID g_gcWhite;
GR_GC_ID g_gcBlack;
GR_WINDOW_ID g_main;

void ClearScreen();
int GetTime();
void delay(unsigned int time);
void printNr(char* buffer, int nr, int withSign);

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
		wmprops.title = "Viewer";
		GrSetWMProperties(g_main, &wmprops);
	}

	GrSelectEvents(g_main, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	g_gcWhite = GrNewGC();
	GrSetGCForeground(g_gcWhite, GR_COLOR_WHITE);
	GrSetGCBackground(g_gcWhite, GR_COLOR_WHITE);

	g_gcBlack = GrNewGC();
	GrSetGCForeground(g_gcBlack, GR_COLOR_WHITE);
	GrSetGCBackground(g_gcBlack, GR_COLOR_BLACK);
	

	GrMapWindow(g_main);

   GrDrawImageFromFile(g_main, g_gcBlack, 10, 10, 200, 100, "/mnt/img.jpg",0);
printf("image loaded\n");
	while (1)
	{
		GR_EVENT event;

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

//   GrFillRect(g_main, g_gc, xPos, 0, 4, LCD_HOEHE);

/*
	for(i = 0; i < 2; i++)
	{
     GrSetGCForeground(g_gcWhite, RGB(0,255,0));//GrGetSysColor(i));

     GrFillRect(g_main, g_gcWhite, xPos, 0, 4, LCD_HOEHE);

	  xPos+=5;
	}*/
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
