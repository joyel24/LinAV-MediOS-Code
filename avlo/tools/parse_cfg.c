/*
*   loader/parse_cfg.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdio.h>
#include <stdlib.h>

#include "parse_cfg.h"

FILE * file;

int line_num;
int back = 0; /* can go back by one char */

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

int file_open(char *name)
{
    if (!(file = fopen(name,"r")))
    {
    	printf("error reading config file %s\n",name);
	return 0;
    }
    line_num = 1;
    back = 0;
    return 1;
}

void file_close()
{
	fclose(file);
}

char next_char(void)
{
    char ch;

    if (!back)
    {
        ch=fgetc(file);
        if(feof(file))
            return 0;
        //printf("read: %c %x\n",ch,ch);
	return ch;

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
    char ch;

/* processing empty lines and comments */
    while (1) {
	while ((ch = next_char()), ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
        {
            //printf("processing %c, it is a space, tab or return \n",ch);
	    if (ch == '\n' || ch == '\r')
            {
               // printf("return => inc line num\n");
                line_num++;
            }
        }
	if (feof(file))
        {
            //printf("eof loop1 => return 0\n");
            return 0;
        }
	if (ch != '#')
        {
            //printf("char %c <> # => break loop 1\n",ch);
            break;
        }
        
	while ((ch = next_char()), (ch != '\n' && ch != '\r'))
        {
            //printf("processing comment %c\n",ch);
	    if(feof(file)) return 0;
        }
        //printf("end of comment => loop again\n");
	line_num++;
    }
    //printf("found an interesting char : %c\n",ch);
/* processing '=' char */    
    if (ch == '=')
    {
        //printf("it's a '=' => return it\n");
    	buff[0]='=';
	buff[1]=0;
    	return 1;
    }
/* processing quoted string */
    if (ch == '"') {
        //printf("start of a quote\n");
	here = buff;
	while (here-buff < MAX_TOKEN) {
	    ch = next_char();
	    if (feof(file))
            {
                printf("EOF in quoted string");
                return 0;
            }
	    if (ch == '"') {
		*here = 0;
                //printf("found end of quoted string : %s\n",buff);
		return 1;
	    }
	    if (ch == '\n' || ch == '\r' || ch == '\t')
		printf("\\n and \\t are not allowed in quoted strings");
	    *here++ = ch;
	}
	printf("Quoted string is too long");
	return 0; /* not reached */
    }
/* processing normal char buff-> start of string, here-> pointer to current char*/
    here = buff;
    //printf("std process of char : %c\n",ch);
    while (here-buff < MAX_TOKEN) {
        //printf("current char: %c %d\n",ch,ch);
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '#' ||
	      ch == '=' || feof(file)) {
		again(ch);
		*here = 0;
                //printf("end of current item: %s\n",buff);
		return 1;
	    }
	*here++ = ch;
	ch = next_char();
    }
    printf("Token is too long: %s\n",buff);
    return 0; /* not reached */
}

int nxt_cfg(char *item,char *value)
{    
    if (!nxt_token(item)) return 0;
    
    printf("[nxt_cfg]: item=|%s|\n",item);
    
    if (!strcmp(item,"="))
    {
    	printf("Syntax error");
	return 0;
    }
    
    if (!nxt_token(value)) return 0;
    if (strcmp(value,"="))
    {
        printf("Error '=' expected (get: %s - l:%d)\n",value,line_num);
        return 0;
    }
    if (!nxt_token(value))
    {
    	printf("Value expected");
	return 0;
    }
    
    printf("[nxt_cfg]: value=|%s|\n",value);
    
    if (!strcmp(value,"="))
    {
    	printf("Syntax error after %s",item);
	return 0;
    }
    return 1;
}





