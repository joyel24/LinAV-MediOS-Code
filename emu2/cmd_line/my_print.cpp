/* 
*   my_print.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define line_length  0x10

#include <mem_space.h>
#include <HW_mem.h>

void print_nonhexa(char * str)
{
    int i;
    for(i=0;i<line_length;i++)
    {
        if(isprint(str[i]))
            printf("%c",str[i]);
        else
            printf(".");
    }
}

void my_print_data(char * data,int length)
{    
    char str[line_length+1];
    int i;
    printf("*************************** %08x - %x\n",data,length);
    for(i=0;i<length;i++)
    {
        if(i%line_length==0) // new line
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
        str[i%line_length]=data[i];        
    }
    printf(" ] ");
    print_nonhexa(str);
    printf("\n");
    printf("***************************\n");
}

void my_print_mem(mem_space * mem_data,char * data,int length)
{    
    char str[line_length+1];
    
    int i;
    printf("*************************** %08x - %x\n",data,length);
    for(i=0;i<length;i++)
    {
        if(i%line_length==0) // new line
        {
            if(i!=0) /* do we need to end the previous line? */
            {
                printf(" ] ");
                print_nonhexa(str);
                printf("\n");
            }
            printf("%08x: [",data+i);
        }
        
        if(i%4==0)
            printf(" ");
            
        str[i%line_length] = (char)mem_data->read((uint32_t)data+i,2);
        printf("%02X",(unsigned char)str[i%line_length]);        
    }
    printf(" ] ");
    print_nonhexa(str);
    printf("\n");
    printf("***************************\n");
}

void my_print_memBuffer(HW_mem * memBuff,int start,int length)
{    
    char str[line_length+1];
    
    int i;
    printf("*************************** %08x - %x\n",start,length);
    for(i=0;i<length;i++)
    {
        if(i%line_length==0) // new line
        {
            if(i!=0) /* do we need to end the previous line? */
            {
                printf(" ] ");
                print_nonhexa(str);
                printf("\n");
            }
            printf("%08x: [",start+i);
        }
        
        if(i%4==0)
            printf(" ");
            
        str[i%line_length] = (char)memBuff->read(start+i,2);
        printf("%02X",(unsigned char)str[i%line_length]);        
    }
    printf(" ] ");
    print_nonhexa(str);
    printf("\n");
    printf("***************************\n");
}
