#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "parse_cfg.h"

FILE * file;

int cfg_line_num;
int back = 0; /* can go back by one char */

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
	    if (ch == '\n' || ch == '\r') cfg_line_num++;
	if (feof(file)) return 0;
	if (ch != '#') break;
	while ((ch = next_char()), (ch != '\n' && ch != '\r'))
	    if (feof(file)) return 0;
	cfg_line_num++;
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

int openFile(char * filename,int mode)
{
    char * file_mode;
    switch(mode)
    {
        case CFG_READ:
            file_mode="r";
            break;
        case CFG_WRITE:
            file_mode="w";
            break;
    }
    if ((file = fopen(filename,file_mode))==NULL)
    {
        fprintf(stderr,"error reading config file %s (%s)\n",filename,file_mode);
        return -1;
    }
    cfg_line_num=1;
    return 0;
}

void closeFile(void)
{
    fclose(file);
}

int write_cfg(char * item,char * value)
{
    char * fmt;
    if(strchr(value,' '))
        fmt="%s=\"%s\"\n";
    else
        fmt="%s=%s\n";
    fprintf(file,fmt,item,value);
    return 1;
}

int write_comment(char * cmt)
{
    fprintf(file,"#%s\n",cmt);
    return 1;
}

int add_line(void)
{
    fprintf(file,"\n");
    return 1;
}

int curLineNum(void) {return cfg_line_num;}



