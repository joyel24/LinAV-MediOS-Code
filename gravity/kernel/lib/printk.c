/* 
*   kernel/printk.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdarg.h>
#include <sys_def/string.h>
#include <sys_def/ctype.h>

#include <kernel/config.h>
#include <kernel/uart.h>

#ifdef USE_DEBUG_ON_SCREEN
#include <kernel/kgraphics.h>
#include <kernel/font.h>
#include <kernel/colordef.h>

#define MAX_COL      78
#define MAX_LINE     38

#define BASE_X       2
#define BASE_Y       2

needFont(std4x6);

static int cur_line = 0;
static int cur_col = 0;

static int fontH,fontW;

void printOnScreen(char * str);
void ini_debugOnScreen(void);
#endif

//int vsnprintf (char * buf, size_t size, const char * fmt, va_list args);
static char debugmembuf[255];

void printk(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    va_end(ap);
    uartOutString(debugmembuf,DEBUG_UART);
#ifdef USE_DEBUG_ON_SCREEN
    printOnScreen(debugmembuf);
#endif
}

void print_nonhexa(char * str)
{
    int i;
    for(i=0;i<16;i++)
    {
        if(isprint(str[i]))
            printk("%c",str[i]);
        else
            printk(".");
    }
}

void print_data(char * data,int length)
{    
    char str[17];
    int i;
    for(i=0;i<length;i++)
    {
        if(i%16==0) // new line
        {
            if(i!=0) /* do we need to end the previous line? */
            {
                printk(" ] ");
                print_nonhexa(str);
                printk("\n");
            }
            printk("%p: [",&data[i]);
        }
        
        if(i%4==0)
            printk(" ");
        
        printk("%02X",(unsigned char)data[i]);
        str[i%16]=data[i];        
    }
    printk(" ] ");
    print_nonhexa(str);
    printk("\n");
}

#ifdef USE_DEBUG_ON_SCREEN
void addLine(void)
{
    cur_col=0;
    cur_line++;
    if(cur_line > MAX_LINE)
    {
        cur_line--;
        scroll_error_scr(COLOR_BLACK,BASE_X,BASE_Y,fontH,1);
    }
}

void printOnScreen(char * str)
{
    while(*str)
    {
        if(*str=='\r' || *str=='\n')
        {
            addLine();
            str++;
        }
        else
        {
            putC_error_scr(std4x6,COLOR_WHITE, COLOR_BLACK, BASE_X+cur_col*fontW, BASE_Y+cur_line*fontH, *str);
            str++;
            cur_col++;
            if(cur_col > MAX_COL)
                addLine();
        }
    }
}

void ini_debugOnScreen(void)
{
    cur_line = 0;
    cur_col = 0;
    fontW=std4x6->width;
    fontH=std4x6->height;
    clear_error_scr(COLOR_BLACK);
}
#endif
