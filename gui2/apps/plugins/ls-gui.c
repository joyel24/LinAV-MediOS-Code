#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

#include "graphics.h"
#include "events.h"
#include "avwm.h"

#define SHOW_ALL        1
#define	LISTSIZE        256
#define	PATHLEN         256

int namesort(s1,s2)
char **s1;
char ** s2;
{
	return strcmp(*s1, *s2);
}

char            **list;
int             listsize;
int             listused;

struct client_operations * cops;

int doLs(char * name)
{	
	DIR             *dirp;
	struct dirent   *dp;
	char            fullname[PATHLEN];
	int             endslash;
	char            **newlist;	
	int             i;
	char            *cp;
	
	endslash = (*name && (name[strlen(name) - 1] == '/'));

	if (listsize == 0) {
		list = (char **) malloc(LISTSIZE * sizeof(char *));
		if (list == NULL) {
			fprintf(stderr, "No memory for ls buffer\n");
			return;
		}
		listsize = LISTSIZE;
	}
	listused = 0;
	
	dirp = opendir(name);
	if (dirp == NULL) {
		perror(name);
		return -1;
	}
	
	
	
	while ((dp = readdir(dirp)) != NULL) {
		if ((dp->d_name[0] == '.') && !SHOW_ALL)
			continue;

		fullname[0] = '\0';

		if ((*name != '.') || (name[1] != '\0')) {
			strcpy(fullname, name);
			if (!endslash)
				strcat(fullname, "/");
		}

		strcat(fullname, dp->d_name);

		if (listused >= listsize) {
			newlist = malloc((sizeof(char **)) * (listsize + LISTSIZE));
			if (newlist == NULL) {
				fprintf(stderr, "No memory for ls buffer\n");
				break;
			}
			memcpy(newlist, list, sizeof(char**) * listsize);
			free(list);
			listsize += LISTSIZE;
		}

		list[listused] = strdup(fullname);
		if (list[listused] == NULL) {
			fprintf(stderr, "No memory for filenames\n");
			break;
		}
		listused++;
	}

	closedir(dirp);

	qsort((char *) list, listused, sizeof(char *), namesort);

	return 0;
}

#define MAXPOS       10

#define WHITE        0
#define BLACK        1
#define GRAY         21
#define BLUE         32
#define RED          77

#define FONT_HEIGHT  10

needFont(std6x9);

int printName(char * name,int x,int y,int clear,int selected)
{
	struct stat     statbuf;
	int             color;
	char *          cp;
	
	cp = strrchr(name, '/');
	if (cp)
		cp++;
	else
		cp = name;
		
	if (stat(name, &statbuf) < 0) {
		perror(name);
		return;
	}
	
	if(S_ISDIR(statbuf.st_mode))
		color=RED;
	else
		color=BLACK;
		
	if(clear)
		cops->fillRect(WHITE,x, y , 315, 10);
		
	if(selected)
		cops->putS(color, BLUE,x, y, name);
	else
		cops->putS(color, WHITE,x, y, name);
}

void printAllName(int pos,int nselect)
{
	int i;
	for (i = pos; i < listused && i < pos+MAXPOS; i++) {
		printName(list[i],5,(i-pos)*FONT_HEIGHT+15,0,(i-pos)==nselect);
	}
}

void printAName(int pos, int nselect, int clear, int selected)
{
	printName(list[pos],5,nselect*FONT_HEIGHT+15,clear,selected);
}

void cleanList()
{
	int i;
	for (i = 0; i < listused; i++)
		free(list[i]);
	listused=0;
}

int is_script_type(char *extension)
{
	return strcmp(extension, ".sh") == 0;
}

int is_image_type(char *extension)
{
	return strcmp(extension, ".jpg") == 0
		|| strcmp(extension, ".gif") == 0
		|| strcmp(extension, ".bmp") == 0;
}

int is_mp3_type(char *extension)
{
	return strcmp(extension, ".mp3") == 0;
}

int is_text_type(char * extension)
{
   int retVal = 0;

   if(strcmp(extension, ".txt") == 0)
	   retVal = 1;
   else if(strcmp(extension, ".cfg") == 0)
	   retVal = 1;
   else if(strcmp(extension, ".c") == 0)
	   retVal = 1;
   else if(strcmp(extension, ".cpp") == 0)
	   retVal = 1;
   else if(strcmp(extension, ".h") == 0)
	   retVal = 1;
   else if(strcmp(extension, ".ini") == 0)
	   retVal = 1;
   else if(strcmp(extension, ".csv") == 0)
	   retVal = 1;

	return retVal;
}

int launchBin(char * name)
{
	int pid;

	cops->clearEventQueue();

	pid = vfork();
	if (pid == 0) {
		execl(name,name,(char*)0);
		fprintf(stderr, "exec failed!\n");
		_exit(1);
	}
	else {
		if (pid > 0) {
			int status;
			cops->closeScreen();
			waitpid(pid, &status, 0);
			cops->openScreen();
		}
		else {
			fprintf(stderr, "vfork failed %d\n", pid);
		}
	}

	//execl();
	//fprintf(stderr, "Cannot restart podzilla!\n");
	//exit(1);
}

void launchSoundPlayer(char *name)
{
	int pid;

	cops->clearEventQueue();

	pid = vfork();
	if (pid == 0) {
		execl("/mnt/play", "play", name, 70, 0);
		fprintf(stderr, "exec failed!\n");
		_exit(1);
	}
	else {
		if (pid > 0) {
			int status;
			cops->closeScreen();
			waitpid(pid, &status, 0);
			cops->openScreen();
		}
		else {
			fprintf(stderr, "vfork failed %d\n", pid);
		}
	}
}

void launchViewer(char *name)
{
	int pid;

	cops->clearEventQueue();

	pid = vfork();
	if (pid == 0) {
		execl("/mnt/viewer", "viewer", name, 70, 0);
		fprintf(stderr, "exec failed!\n");
		_exit(1);
	}
	else {
		if (pid > 0) {
			int status;
			cops->closeScreen();
			waitpid(pid, &status, 0);
			cops->openScreen();
		}
		else {
			fprintf(stderr, "vfork failed %d\n", pid);
		}
	}
}

int launchScript(char * name)
{
	char tmp[1000];
	int pid;

	cops->clearEventQueue();
	
	pid = vfork();
	if (pid == 0) {
		execl("/bin/sh", "sh", name);
		fprintf(stderr, "exec failed!\n");
		_exit(1);
	}
	else {
		if (pid > 0) {
			int status;
			cops->closeScreen();
			waitpid(pid, &status, 0);
			cops->openScreen();
		}
		else {
			fprintf(stderr, "vfork failed %d\n", pid);
		}
	}

	//execl();
	//fprintf(stderr, "Cannot restart podzilla!\n");
	//exit(1);
}

void handle_type_other(char *filename)
{
	char *ext;

	ext = strrchr(filename, '.');
	if (ext == 0)
	{
      // no extension
		launchBin(filename);
	}
	else if (is_image_type(ext))
	{
	   launchViewer(filename);
	}
	else if (is_text_type(ext))
	{
	}
	else if (is_mp3_type(ext))
	{
	   launchSoundPlayer(filename);
	}
	else if (is_script_type(ext))
	{
	   launchScript(filename);
	}
	else
	{
	   // unknown type
	}
}


int pos,nselect,stop;

int eventHandler(int evt)
{
	/*char tmp[50];
	sprintf(tmp,"in handler (%d)",evt);
	cops->putS(BLACK, WHITE,0, 220, tmp);*/
	
	switch(evt) {
		case BTN_UP:
			nselect--;
			if(nselect<0)
			{
				nselect=0;
				pos--;
				if(pos<0) // we are at the beg => can't go up anymore
					pos=0;
				else // not going up, scrolling
				{
					cops->scrollWindowVert(WHITE, 5, 15, 315, FONT_HEIGHT*MAXPOS, FONT_HEIGHT,0);
					printAName(pos+nselect+1,nselect+1,1,0);
					printAName(pos+nselect,nselect,1,1);
				}
			}
			else // just going up
			{
				printAName(pos+nselect+1,nselect+1,0,0);
				printAName(pos+nselect,nselect,0,1);
			}
			break;
		case BTN_DOWN:
			nselect++;
			if(nselect+pos>=listused)
				nselect--;
			if(nselect>=MAXPOS)
			{
				nselect=MAXPOS-1;
				pos++;
				if(pos>=(listused-MAXPOS)) // we are at the end => can't go down anymore
					pos=listused-MAXPOS-1;
				else // not going down, scrolling
				{
					cops->scrollWindowVert(WHITE, 5, 15, 315, FONT_HEIGHT*MAXPOS, FONT_HEIGHT,1);
					printAName(pos+nselect-1,nselect-1,1,0);
					printAName(pos+nselect,nselect,1,1);
				}
			}
			else
			{
				printAName(pos+nselect-1,nselect-1,0,0);
				printAName(pos+nselect,nselect,0,1);
			}
			break;
		case BTN_RIGHT:
			if(chdir(list[pos+nselect])<0)
				handle_type_other(list[pos+nselect]);
			cleanList();
			if(doLs("./")<0)
			{
				listused = 0;
				return -1;
			}
			pos=0;
			nselect=0;
			cops->fillRect(WHITE,5, 15 , 315,FONT_HEIGHT*MAXPOS);
			printAllName(pos,nselect);
			cops->clearEventQueue();
			break;
		case BTN_LEFT:
			if(chdir("../")<0)
				break;
			cleanList();
			if(doLs("./")<0)
			{
				listused = 0;
				return -1;
			}
			pos=0;
			nselect=0;
			cops->fillRect(WHITE,5, 15 , 315,FONT_HEIGHT*MAXPOS);
			printAllName(pos,nselect);
			cops->clearEventQueue();
			break;
		case BTN_OFF:
		case EVT_QUIT:
			stop=1;
			break;
		case EVT_REDRAW:
			cops->fillRect(WHITE,5, 15 , 315,225);
			printAllName(pos,nselect);
			break;
	}
}

int main(int argc,char * * argv)
{
	int evt;
	REGISTER(cops,eventHandler);
	
	pos=0;
	nselect=0;
	stop=0;
	
	if(argc>0)
	{
		listused = 0;
		
		chdir(argv[1]);
		
		if(doLs("./")<0)
		{
			listused = 0;
			return -1;
		}
		cops->fillRect(WHITE,5, 15 , 320,225);
		printAllName(pos,nselect);
		
		PACK(cops);
		while(!stop) /*wait */
		{
			//while((evt=cops->nxtEvent())==NO_EVENT) /* wait */;
			//eventHandler(evt);
			
		}
		
		cleanList();
		return 0;
	}
	
	return -1;
}
