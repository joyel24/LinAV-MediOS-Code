#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

#include "graphics.h"
#include "events.h"
#include "avwm.h"
#include "parse_cfg.h"
#include "menu.h"
#include "colordef.h"

#define SHOW_ALL        1
#define	LISTSIZE        256
#define	PATHLEN         256

#define MAXPOS       10
#define TITLE_OFFSET  2

//#define FONT_HEIGHT  10 // now using graphic function for that

struct cfg_menu * cfgMenu=NULL;;
struct menu_item * rootMenu=NULL;

int stop,nselect;
struct menu_item *pos;
struct menu_item *pselect;

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

struct cfg_menu * current_item=NULL;

extern int cfg_line_num;

void cleanMenu(struct menu_item * root)
{
	struct menu_item * ptr;
	while(root!=NULL)
	{
		cleanMenu(root->sub);
		ptr=root->nxt;
		free(root);
		root=ptr;
	}
}

struct menu_item * newItem(struct cfg_menu * data)
{
	struct menu_item * ptr=(struct menu_item *) malloc(sizeof(struct menu_item));
	if(ptr)
	{
		ptr->data=data;
		ptr->nxt=NULL;
		ptr->prev=NULL;
		ptr->sub=NULL;
		ptr->up=NULL;
	}
	else
		fprintf(stderr,"Not enough space in memory, cant malloc\n");
	return ptr;
}

struct menu_item * findParent(struct menu_item * ptr, char * name)
{
	struct menu_item * ptr2;
	while(ptr!=NULL)
	{
		if(!strcmp(ptr->data->name,name))
			return ptr;
		if((ptr2=findParent(ptr->sub,name))!=NULL)
			return ptr2;
		ptr=ptr->nxt;
	}
	return NULL;
	
}

int insertItem(struct menu_item * item)
{
	struct menu_item * ptr;
	if(rootMenu==NULL)
	{
		if(item->data->parent[0] != 0) 
			return -1; // no sub defined and data is not in root => error
		else
			rootMenu=item; // no root => item is first item
	}
	else
	{
		if(item->data->parent[0] == 0) // no parent => add it on top of root
		{
			rootMenu->prev=item;
			item->nxt=rootMenu;
			rootMenu=item;
		}
		else
		{
			if((ptr=findParent(rootMenu,item->data->parent))!=NULL)
			{
				if(ptr->sub)
					ptr->sub->prev=item;
				item->nxt=ptr->sub;
				ptr->sub=item;
				item->up=ptr;
			}
			else
				return -1; // parent not found
		}
	}
	return 0;
}

void addItem(struct cfg_menu ** cfg)
{
	struct cfg_menu * ptr =(struct cfg_menu *) malloc(sizeof(struct cfg_menu));
	if(current_item == NULL)
		*cfg=ptr;
	else
		current_item->nxt=ptr;
	current_item=ptr;
	current_item->name[0]=0;
	current_item->link[0]=0;
	current_item->parent[0]=0;
	current_item->param[0]=0;
}

void cfgCleanMenu(struct cfg_menu * cfg)
{
	struct cfg_menu * ptr;
	while(cfg!=NULL)
	{
		ptr=cfg->nxt;
		free(cfg);
		cfg=ptr;
	}
}

int do_parse(struct cfg_menu ** cfg,char * filename)
{
    char *item=item_buff;
    char *value=value_buff;

    openFile(filename);

    while (1) {
	if (!nxt_cfg(item,value)) break;
	if(!strcmp(item,"name"))
	{
		addItem(cfg);
		strcpy(current_item->name,value);
	}
	else if(!strcmp(item,"parent"))
	{
		if(current_item==NULL)
		{
			fprintf(stderr,"'label' param before image\n");
		}
		else
		{
			strcpy(current_item->parent,value);
		}

	}
	else if(!strcmp(item,"link"))
	{
		if(current_item==NULL)
		{
			fprintf(stderr,"'link' param before image\n");
		}
		else
		{
			strcpy(current_item->link,value);
		}
	}
	else if(!strcmp(item,"param"))
	{
		if(current_item==NULL)
		{
			fprintf(stderr,"'param' param before image\n");
		}
		else
		{
			strcpy(current_item->param,value);
		}
	}
	else
		fprintf(stderr,"unknown item type: %s on line %d\n",item,cfg_line_num);
    }
    closeFile();
    return 0;
}

int loadMenu(char * filename)
{
	struct cfg_menu * data;
	struct menu_item * new_item;
        fprintf(stderr,"Reading: %s\n",filename);
	cfgCleanMenu(cfgMenu);
	cfgMenu=NULL;
	cleanMenu(rootMenu);
	rootMenu=NULL;
	if(do_parse(&cfgMenu,filename)<0)
		return -1;
	data=cfgMenu;
	while(data!=NULL)
	{
		if(!(new_item=newItem(data)))
			return -1;
		if(insertItem(new_item)<0)
		{
			/*cfgCleanMenu(cfgMenu); !!!!!!!!!!!!! do clean when everything is working
			cleanMenu(rootMenu);*/
			fprintf(stderr,"Error building menu tree\n");
			return -1;
		}
		data=data->nxt;
	}
	return 0;
}

char tmp[MAX_TOKEN+5];

int printName(struct menu_item * item,int x,int y,int clear,int selected)
{
	int color;
   int w = 0;
	int h = 0;

   getStringS("M", &w, &h);

	if(item->sub)
	{
		sprintf(tmp,"> %s",item->data->name);
		color=COLOR_RED; // => submenu
	}
	else
	{
		sprintf(tmp,"x %s",item->data->name);
		color=COLOR_BLACK; // => item
	}
		
	if(clear)
		fillRect(COLOR_WHITE,x, y , 310, h+1);



	if(selected)
		putS(color, COLOR_BLUE,x, y, tmp);
	else
		putS(color, COLOR_WHITE,x, y, tmp);
}

void printAllName(struct menu_item * pos,int nselect)
{
	struct menu_item * i;
	int nbAff=0;
   int w = 0;
	int h = 0;

   getStringS("M", &w, &h);

	for (i = pos; i !=NULL && nbAff < MAXPOS; i=i->nxt) {
		printName(i,5,TITLE_OFFSET + nbAff*(h+1) + h+6+MENU_SHADOW,0,nbAff==nselect);
		nbAff++;
	}
}

void printAName(struct menu_item * pos, int posY, int clear, int selected)
{
   int w = 0;
	int h = 0;

   getStringS("M", &w, &h);

	printName(pos,5,TITLE_OFFSET + posY*(h+1)+ h+6+MENU_SHADOW,clear,selected);
}

extern int stopWM;

int eventHandler(int evt)
{
   int w = 0;
	int h = 0;

   getStringS("M", &w, &h);

	switch(evt) {
		case BTN_UP:
			if(nselect==0) // moving out of current window
			{
				if(!pos->prev) // we are at the beg => nothing to change
					break; // to do rolling menu code to change is here

				pos=pos->prev;
				pselect=pos;

				scrollWindowVert(COLOR_WHITE, 5, h+6+MENU_SHADOW, 315, (h+1)*MAXPOS, h+1,0);
			}
			else // just going up
			{
				nselect--;
				pselect=pselect->prev;
			}
			printAName(pselect->nxt,nselect+1,1,0);
			printAName(pselect,nselect,1,1);
			break;
		case BTN_DOWN:
			if(!pselect->nxt) // we are at the end => can't go down anymore
				break;

			if(nselect==MAXPOS-1) // moving out of the window
			{
				if(!pos->nxt) // we are at the end => can't go down anymore
					break;
				
				pos=pos->nxt;
				pselect=pos;
				
				scrollWindowVert(COLOR_WHITE, 5, h+6+MENU_SHADOW, 315, (h+1)*MAXPOS, h+1,1);
			}
			else
			{
				nselect++;
				pselect=pselect->nxt;
			}
			printAName(pselect->prev,nselect-1,1,0);
			printAName(pselect,nselect,1,1);
			break;
		case BTN_RIGHT:
			if(pselect->sub) // submenu
			{
				pos=pselect->sub;
				nselect=0;
				pselect=pos;
				fillRect(COLOR_WHITE,5, h+6+MENU_SHADOW , 315,(h+1)*MAXPOS);
				printAllName(pos,nselect);
				clearEventQueue();
			}
			else // launch plugin
			{
				if(pselect->data->link[0]!=0)
				{
					if(pselect->data->param[0]!=0)
					{
						if(loadPlugin(pselect->data->link,pselect->data->param)>=0)
							stop=1;
					}
					else
					{
						if(loadPlugin(pselect->data->link,NULL)>=0)
							stop=1;
					}
				}
			}
			break;
		case BTN_LEFT:
			if(pselect->up)
			{
				if(pos->up)
				{
					if(pos->up->up)
						pos=pos->up->up;	
					else
						pos=rootMenu;
					nselect=0;
					pselect=pos;
					fillRect(COLOR_WHITE,5, h+6+MENU_SHADOW , 315,(h+1)*MAXPOS);
					printAllName(pos,nselect);
					clearEventQueue();
				}
			}
			break;
		case BTN_OFF:
			stopWM=1;
			stop=1;
			break;
	}
	return 0;
}

extern int timerOn;

void doDraw()
{
	int evt;
   int w = 0;
	int h = 0;

   getStringS("M", &w, &h);

	pos=rootMenu;
	pselect=rootMenu;
	nselect=0;
	stop=0;

	fillRect(COLOR_WHITE,5, h+6+MENU_SHADOW/*20*/, 315,(h+1)*MAXPOS);
	printAllName(pos,nselect);


	while(!stop) /*wait */
	{
		evt=waitEvent();
		if(timerOn && evt==EVT_TIMER)
		{
			processTimeOut();
		}

		eventHandler(evt);
	}
}

void doPrint(struct menu_item * ptr,int level)
{
	int i;
	while(ptr!=NULL)
	{
		for(i=0;i<level;i++)
			printf("  ");
		if(ptr->sub)
		{
			printf("%s ->\n",ptr->data->name);
			doPrint(ptr->sub,level+1);
		}
		else
			printf("%s\n",ptr->data->name);
		ptr=ptr->nxt;
	}
}

void printMenu(void)
{
	struct cfg_menu * ptr=cfgMenu;
	printf("cfg:\n");
	while(ptr)
	{
		printf("%s %s %s\n",ptr->name,ptr->parent,ptr->link);
		ptr=ptr->nxt;
	}
	printf("Menu:\n");
	doPrint(rootMenu,0);
}


