#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

#include "graphics.h"
#include "events.h"

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
		color=GRAY;
		
	if(clear)
		fillRect(BLACK,x, y , 320, 10);
		
	if(selected)
		putS(color, BLUE,x, y, name);
	else
		putS(color, BLACK,x, y, name);
}

void printAllName(int pos,int select)
{
	int i;
	for (i = pos; i < listused && i < pos+MAXPOS; i++) {
		printName(list[i],5,(i-pos)*FONT_HEIGHT+5,0,(i-pos)==select);
	}
}

void printAName(int pos, int select, int clear, int selected)
{
	printName(list[pos],5,select*FONT_HEIGHT+5,clear,selected);
}

void cleanList()
{
	int i;
	for (i = 0; i < listused; i++)
		free(list[i]);
	listused=0;
}

int main(int argc,char * * argv)
{
	char * name;
	int pos=0;
	int select=0;
	int evt;
	int stop=0;
	
	if(argc>1)
	{
		listused = 0;
		
		chdir(argv[1]);
		
		if(doLs("./")<0)
		{
			listused = 0;
			return -1;
		}
		
		ini_graphics();
		setFont(std6x9);
		fillRect(BLACK,0, 0 , 320, 240);
		
		printAllName(pos,select);
		
		while(!stop)
		{
			while((evt=nxtEvent())==NO_EVENT) /* nothing */;
			
			switch(evt) {
				case BTN_UP:
					select--;
					if(select<0)
					{
						select=0;
						pos--;
						if(pos<0) // we are at the beg => can't go up anymore
							pos=0;
						else // not going up, scrolling
						{
							scrollWindowVert(-1, 5, 5, 315, FONT_HEIGHT*MAXPOS, FONT_HEIGHT,0);
							printAName(pos+select+1,select+1,1,0);
							printAName(pos+select,select,1,1);
						}
					}
					else // just going up
					{
						printAName(pos+select+1,select+1,0,0);
						printAName(pos+select,select,0,1);
					}
					break;
				case BTN_DOWN:
					select++;
					if(select+pos>=listused)
						select--;
					if(select>=MAXPOS)
					{
						select=MAXPOS-1;
						pos++;
						if(pos>=(listused-MAXPOS)) // we are at the end => can't go down anymore
							pos=listused-MAXPOS-1;
						else // not going down, scrolling
						{
							scrollWindowVert(-1, 5, 5, 315, FONT_HEIGHT*MAXPOS, FONT_HEIGHT,1);
							printAName(pos+select-1,select-1,1,0);
							printAName(pos+select,select,1,1);
						}
					}
					else
					{
						printAName(pos+select-1,select-1,0,0);
						printAName(pos+select,select,0,1);
					}
					break;
				case BTN_RIGHT:
					if(chdir(list[pos+select])<0)
						break;
					cleanList();
					if(doLs("./")<0)
					{
						listused = 0;
						return -1;
					}
					pos=0;
					select=0;
					fillRect(BLACK,5, 5 , 320,FONT_HEIGHT*MAXPOS);
					printAllName(pos,select);
					clearEventQueue();
					break;
				case BTN_OFF:
					stop=1;
				break;
			}
		}
		
		listused = 0;
		close_graphics();
		return 0;
	}
	
	return -1;
}