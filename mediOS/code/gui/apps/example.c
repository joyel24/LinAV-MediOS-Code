#include <stdio.h>
#include <stdlib.h>
#define MWINCLUDECOLORS
#include "nano-X.h"

int COLS, ROWS;
GR_WINDOW_ID g_main;

static void
draw_screen(int i)
{
	GR_POINT tri[4] = { {5, 115}, {105, 115}, {55, 200}, {5, 115} };
	GR_WINDOW_INFO winfo;
	GR_GC_ID gc;
	char dash1[2] = { 10, 5 };
	char dash2[4] = { 5, 2, 1, 2 };
	char dash3[4] = { 5, 2, 5, 5 };
	char dash4[2] = { 2, 2 };

	GrGetWindowInfo(g_main, &winfo);

	/* Draw several lines and a few boxes */
	gc = GrNewGC();
	GrSetGCLineAttributes(gc, GR_LINE_ONOFF_DASH);

	/* Draw a dashed box */
	if(i==1)
	{
		GrSetGCDash(gc, dash1, 2);
		GrRect(g_main, gc, 5, 5, 100, 100);
	}

	if(i==2)
	{
		GrSetGCDash(gc, dash2, 4);
		GrLine(g_main, gc, 10, 10, 95, 95);
	}

	if(i==3)
	{
		GrSetGCDash(gc, dash3, 4);
		GrEllipse(g_main, gc, 160, 55, 50, 50);
	}
	
	if(i==4)
	{
		GrSetGCDash(gc, dash4, 2);
		GrPoly(g_main, gc, 4, tri);
	}

	if(i==5)
		GrText(g_main, gc, 8, 20, "Press action to playback", 2, GR_TFASCII);

	GrDestroyGC(gc);
}

int
main(int argc, char **argv)
{
	int COLS, ROWS;

	GR_WM_PROPERTIES wmprops;

	if (GrOpen() < 0) {
		fprintf(stderr, "Cannot open graphics\n");
		exit(1);
	}



	COLS = 320;
	ROWS = 240;

	/*g_main = GrNewWindowEx(GR_WM_PROPS_APPWINDOW, "dashdemo",
		GR_ROOT_WINDOW_ID, 20, 50, COLS - 120, ROWS - 60, BLUE);*/
	g_main = GrNewWindow(GR_ROOT_WINDOW_ID, 20,50, COLS - 120, ROWS - 60, 2, RED, GREEN);

	if(g_main)
	{
		wmprops.flags = GR_WM_FLAGS_PROPS | GR_WM_FLAGS_TITLE;
		wmprops.props = GR_WM_PROPS_APPWINDOW;
		wmprops.title = "dashdemo";
		GrSetWMProperties(g_main, &wmprops);
	}
	
	//draw_screen(1);

	GrSelectEvents(g_main, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_CLOSE_REQ|GR_EVENT_MASK_KEY_DOWN);
	GrMapWindow(g_main);
	
	//GrClearWindow(g_main,0);
	
	//new_message_window("could not create thread");

	while (1) {
		GR_EVENT event;
		GrGetNextEvent(&event);

		switch (event.type) {
		case GR_EVENT_TYPE_EXPOSURE:
			break;

		case GR_EVENT_TYPE_CLOSE_REQ:
			GrClose();
			exit(0);
		case GR_EVENT_TYPE_KEY_DOWN:
			switch (event.keystroke.ch)
			{
				case 'l':
					draw_screen(1);
					break;
				case 'u':
					draw_screen(2);
					break;
				case 'r':
					draw_screen(3);
					break;
				case 'd':
					draw_screen(4);
					break;
				case '2':
					draw_screen(5);
					break;	
				
			}
			break;
		}
	}
}
