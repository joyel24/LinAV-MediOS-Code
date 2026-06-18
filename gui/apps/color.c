
#include <stdio.h>
#include <stdlib.h>
#define MWINCLUDECOLORS
#include "nano-X.h"
#include "nxcolors.h"
#include "av3xx_colordef.h"

#define NUM_PARTICLES 320

#define LCD_HOEHE   240
#define LCD_BREITE  320

#define FONTNAME GR_FONT_SYSTEM_VAR

GR_GC_ID g_gcWhite;
GR_GC_ID g_gcBlack;
GR_GC_ID g_gcText;

GR_FONT_ID fontid;

GR_WINDOW_ID g_main;

void ClearScreen();
void ShowColors();


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
		wmprops.title = "ColorTest";
		GrSetWMProperties(g_main, &wmprops);
	}

   fontid = GrCreateFont(FONTNAME, 8, NULL);


	GrSelectEvents(g_main, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	g_gcWhite = GrNewGC();
	g_gcBlack = GrNewGC();
	g_gcText  = GrNewGC();

	GrSetGCForegroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);
	GrSetGCBackgroundPixelVal(g_gcBlack, AV3XX_COLOR_BLACK);

	GrSetGCForegroundPixelVal(g_gcWhite, AV3XX_COLOR_WHITE);
	GrSetGCBackgroundPixelVal(g_gcWhite, AV3XX_COLOR_BLACK);

	GrSetGCForegroundPixelVal(g_gcText, AV3XX_COLOR_WHITE);
	GrSetGCBackgroundPixelVal(g_gcText, AV3XX_COLOR_BLACK);
   GrSetGCFont(g_gcText, fontid);
   GrSetFontAttr(fontid, GR_TFKERNING | GR_TFANTIALIAS, 0);


	GrMapWindow(g_main);

   ShowColors();

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
						GrDestroyGC(g_gcText);
          	      GrDestroyFont(fontid);

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

void ShowColors()
{
   char tmp[3];

   int indexColor = 0;
	int xPos = 0;
	int yPos = 0;
	int i = 0;

   ClearScreen();

	while(indexColor < 255)
	{
		for(i = 0; i < 16; i++)
		{
//			GrSetGCForegroundPixelVal(g_gcWhite, indexColor);
//			GrFillRect(g_main, g_gcWhite, xPos, yPos, 20, 15);

      	GrSetGCBackgroundPixelVal(g_gcText, indexColor);

			sprintf(tmp, "%ld", indexColor);
		   GrText(g_main, g_gcText, xPos+2, yPos+11, tmp, strlen(tmp), GR_TFASCII);

			indexColor++;

			xPos+=20;
		}

		xPos = 0;
		yPos += 15;
   }
}
