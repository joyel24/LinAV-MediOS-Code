#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "parse_cfg.h"

FILE * file;

int line_num;
int back = 0; /* can go back by one char */

struct cfg_item * current_item=NULL;

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

int next_char(void)
{
    int ch;

    if (!back) return fgetc(file);
    ch = back;
    back = 0;
    return ch;
}

void again(int ch)
{
    back = ch;
}

int nxt_token(char * buff)
{
    char *here;
    int ch;

/* processing empty lines and comments */
    while (1) {
	while ((ch = next_char()), ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
	    if (ch == '\n' || ch == '\r') line_num++;
	if (feof(file)) return 0;
	if (ch != '#') break;
	while ((ch = next_char()), (ch != '\n' && ch != '\r'))
	    if (feof(file)) return 0;
	line_num++;
    }
/* processing '=' char */    
    if (ch == '=')
    {
    	buff[0]='=';
	buff[1]=0;
    	return 1;
    }
/* processing quoted string */
    if (ch == '"') {
	here = buff;
	while (here-buff < MAX_TOKEN) {
	    ch = next_char();
	    if (feof(file)) fprintf(stderr,"EOF in quoted string");
	    if (ch == '"') {
		*here = 0;
		return 1;
	    }
	    if (ch == '\n' || ch == '\r' || ch == '\t')
		fprintf(stderr,"\\n and \\t are not allowed in quoted strings");
	    *here++ = ch;
	}
	fprintf(stderr,"Quoted string is too long");
	return 0; /* not reached */
    }
/* processing normal char */
    here = buff;
    while (here-buff < MAX_TOKEN) {
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '#' ||
	      ch == '=' || feof(file)) {
		again(ch);
		*here = 0;
		return 1;
	    }
	*here++ = ch;
	ch = next_char();
    }
    fprintf(stderr,"Token is too long");
    return 0; /* not reached */
}

int nxt_cfg(char *item,char *value)
{
    
    if (!nxt_token(item)) return 0;
    if (!strcmp(item,"="))
    {
    	fprintf(stderr,"Syntax error");
	return 0;
    }
    
    if (!nxt_token(value)) return 0;
    if (strcmp(value,"="))
    {
    	fprintf(stderr,"Error '=' expected (get: %s)\n",value);
	return 0;
    }
    if (!nxt_token(value))
    {
    	fprintf(stderr,"Value expected");
	return 0;
    }
    if (!strcmp(value,"="))
    {
    	fprintf(stderr,"Syntax error after %s",item);
	return 0;
    }
    return 1;
}

void addItem(struct cfg_item ** cfg)
{
	struct cfg_item * ptr =(struct cfg_item *) malloc(sizeof(struct cfg_item));
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

int do_parse(struct cfg_item ** cfg,char * filename)
{
    char *item=item_buff;
    char *value=value_buff;
    
    if ((file = fopen(filename,"r"))<0)
    {
    	fprintf(stderr,"error reading config file %s\n",filename);
	return -1;
    }
    line_num = 1;
    
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
		fprintf(stderr,"unknown item type: %s on line %d\n",item,line_num);
    }
    fclose(file);
    return 0;    
}

void cfgCleanMenu(struct cfg_item * cfg)
{
	struct cfg_item * ptr;
	while(cfg!=NULL)
	{
		ptr=cfg->nxt;
		free(cfg);
		cfg=ptr;
	}	
}


