
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#define MWINCLUDECOLORS
#include "nano-X.h"
#include "nxcolors.h"
#include "av3xx_colordef.h"

#define LCD_HOEHE   240
#define LCD_BREITE  320

#define PATH "/mnt/LinAv"

GR_GC_ID g_gcWhite;
GR_GC_ID g_gcBlack;

GR_WINDOW_ID g_main;

struct dispDir {
        char name[14];
        char filename[14];
        int attr;
        char ext[4];
};

static struct dispDir dirBufferHDD[1000];

void ClearScreen();
int GetTime();
void delay(unsigned int time);


int main()
{
	GR_WM_PROPERTIES wmprops;

	DIR* dir = 0;
	struct dirent* entry;
   int totalEntries;
	int i=0;
	int len;
	int xPos = 0;
	int yPos = 0;
   pid_t pid;
	char tmp[1000];

	if (GrOpen() < 0) {
		fprintf(stderr, "Cannot open graphics\n");
		exit(1);
	}

	g_main = GrNewWindow(GR_ROOT_WINDOW_ID, 0,0, LCD_BREITE, LCD_HOEHE, 0, GR_COLOR_BLACK, 0);

	if(g_main)
	{
		wmprops.flags = GR_WM_FLAGS_PROPS | GR_WM_FLAGS_TITLE;
		wmprops.props = GR_WM_PROPS_APPWINDOW;
		wmprops.title = "LinWin";
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

	ClearScreen();

	// Verzeichnis auslesen
   if((dir=opendir(PATH))<0)
   {
		exit(1);
   }

	while((entry=readdir(dir))!=NULL && i<100)
	{
      strcpy(dirBufferHDD[i].name, entry->d_name);

		for(len=strlen(dirBufferHDD[i].name);len<12;len++)
   		dirBufferHDD[i].name[len]=' ';

		dirBufferHDD[i].name[12]=0x0;

		strcpy(dirBufferHDD[i].filename, entry->d_name);
//		strcpy(dirBufferHDD[i].ext, entry->ext);
//		dirBufferHDD[i].attr=entry->attribute;
		i++;
   }

	closedir(dir);
	totalEntries=i;

	xPos = 10;
	yPos = 5;

	for(i = 0; i < totalEntries; i++)
	{
		GrText(g_main, g_gcWhite, xPos, yPos, dirBufferHDD[i].name, strlen(dirBufferHDD[i].name), GR_TFASCII);
		yPos+=15;
   }
/*
void new_mp3_window(char *filename)
 {
     pid_t pid;

     GrClose();

     pid = vfork();
     if (pid == 0) {
         execl("/sbin/mp3example", "mp3example", filename);
         fprintf(stderr, "exec failed!\n");
         exit(1);
     }
     else {
         if (pid > 0) {
             int status;

             waitpid(pid, &status, 0);
         }
         else {
             fprintf(stderr, "vfork failed %d\n", pid);
         }
     }

     execl("/sbin/podzilla", "podzilla");
     fprintf(stderr, "Cannot restart podzilla!\n");
     exit(1);
 }
 */

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

					case '1': // f1 --> select file

  						 GrClose();

					    sprintf(tmp, "%s/%s",PATH, dirBufferHDD[2].filename);
                   GrText(g_main, g_gcWhite, 10, 200, tmp, strlen(tmp), GR_TFASCII);
		   printf("tmp");

                   pid = vfork();
                   if (pid == 0)
						 {
	                    int err = execl(tmp, dirBufferHDD[2].filename);
      				     sprintf(tmp,"Execl-Error:  %d",err);
                       GrText(g_main, g_gcWhite, 10, 5, tmp, strlen(tmp), GR_TFASCII);
		       printf("tmp");
                       fprintf(stderr, "exec failed!\n");
                       exit(1);
                   }
                   else
                   {
//fprintf(stderr, "else !\n");
                       if (pid > 0)
                       {
                           int status;
                           pid_t err2;
//fprintf(stderr, "for waitpid !\n");
                           err2 = waitpid(pid, &status, 0); // warten auf Ende
//fprintf(stderr, "after waitpid %d!\n", (int)err2);
                       }
                       else
							  {
                           fprintf(stderr, "vfork failed %d\n", pid);
                       }
                   }
						 break;

					case '2': // f2 --> select file

					   break;

					case '3': // f3 --> select file

					   break;

					case 'u': // up

					   break;

					case 'd': // down

					   break;
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

