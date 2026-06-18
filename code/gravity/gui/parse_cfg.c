/*
* kernel/gui/parse_cfg.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <sys_def/string.h>

#include <fs_io.h>
#include <api.h>

#include <gui/parse_cfg.h>

int file;

int feof_reached=0;

int cfg_line_num;
int back = 0; /* can go back by one char */

int feof(int file)
{
    return feof_reached;
}

int next_char(void)
{
    int ch;
    char buff[2];

    if (!back)
    {
        if(fread(file,buff,1)!=1)
            feof_reached=1;
        //printf("read: %c\n",buff[0]);
        return buff[0];
    }
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
	    if (feof(file)) printf("EOF in quoted string");
	    if (ch == '"') {
		*here = 0;
                return 1;
	    }
	    if (ch == '\n' || ch == '\r' || ch == '\t')
		printf("\\n and \\t are not allowed in quoted strings");
	    *here++ = ch;
	}
	printf("Quoted string is too long");
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
    printf("Token is too long");
    return 0; /* not reached */
}

int nxt_cfg(char *item,char *value)
{
    
    if (!nxt_token(item)) return 0;
    
    //printf("get item: %s\n",item); 
       
    if (!strcmp(item,"="))
    {
    	printf("Syntax error");
	return 0;
    }   
    
    if (!nxt_token(value)) return 0;
    
    //printf("get item: %s\n",value);
    
    if (strcmp(value,"="))
    
    {
    	printf("Error '=' expected (get: %s)\n",value);
	return 0;
    }
    if (!nxt_token(value))
    {
    	printf("Value expected");
	return 0;
    }
    
    //printf("get item: %s\n",value);
    
    if (!strcmp(value,"="))
    {
    	printf("Syntax error after %s",item);
	return 0;
    }
    return 1;
}

int openFile(char * filename,int mode)
{    
    if ((file = fopen(filename,mode))<0)
    {
        printf("error reading config file %s (%d)\n",filename,mode);
        return -1;
    }
    cfg_line_num=1;
    feof_reached=0;
    return 0;
}

void closeFile(void)
{
    fclose(file);
}

/*
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
}*/

int curLineNum(void) {return cfg_line_num;}



