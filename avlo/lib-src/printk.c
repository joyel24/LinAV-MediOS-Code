/*
*   lib/printf.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <uart.h>
#include <hardware.h>
static char debugmembuf[255];
void printf(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    va_end(ap);
    uartOutString(debugmembuf,DEBUG_UART);
}
void print_nonhexa(char * str)
{
    int i;
    for(i=0;i<16;i++)
    {
        if(isprint(str[i]))
            printf("%c",str[i]);
        else
            printf(".");
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
                printf(" ] ");
                print_nonhexa(str);
                printf("\n");
            }
            printf("%p: [",&data[i]);
        }
        if(i%4==0)
            printf(" ");
        printf("%02X",(unsigned char)data[i]);
        str[i%16]=data[i];
    }
    printf(" ] ");
    print_nonhexa(str);
    printf("\n");
}
