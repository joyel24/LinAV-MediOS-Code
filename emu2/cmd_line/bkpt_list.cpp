/*
*   bkpt_list.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdio.h>
#include <stdlib.h>

#include <emu.h>
#include <cmd_line.h>
#include <bkpt_list.h>


char * bkpt_str[2] = { "CPU", "MEM" };


BKPT_LIST * new_bkpt_list(int type)
{
    BKPT_LIST * ptr = new bkpt_list();
    if(!ptr)
    {
        printf("Can't create bkpt list in memory\n");
        return NULL;
    }
    ptr->head = NULL;
    ptr->type = type;
    ptr->bk_count = 0;
    ptr->fct = fct_void;
    return ptr;
}

void add(BKPT_LIST * ptr,uint32_t address,char * cause)
{
    add(ptr,address,0,cause);
}

void add(BKPT_LIST * ptr,uint32_t address)
{
    add(ptr,address,0,NULL);
}

void add(BKPT_LIST * ptr,uint32_t address,uint32_t size)
{
    add(ptr,address,size,NULL);
}

void add(BKPT_LIST * ptr_list,uint32_t address,uint32_t size,char * cause)
{
    BKPT * ptr;
    BKPT * ptr_new = new BKPT();
    if(!ptr_new)
    {
        printf("Can't create new %s bkpt (new returned a NULL pointer)\n",bkpt_str[ptr_list->type]);
        return;
    }
    ptr_new->address = address;
    ptr_new->size = size;
    ptr_new->cause = cause;

    if(ptr_list->head == NULL || ptr_list->head->address > address) /* list empty or address < => insert at the beg*/
    {
        ptr_new->nxt = ptr_list->head;
        ptr_list->head = ptr_new;
    }
    else                                        /* let's find where to insert */
    {
        ptr=ptr_list->head;
        while(ptr->nxt!=NULL && ptr->nxt->address<address)
            ptr=ptr->nxt;

        ptr_new->nxt = ptr->nxt;
        ptr->nxt = ptr_new;
    }
    ptr_list->bk_count++;
    updateFctPointer(ptr_list);
    printf("adding %s bkpt %d for 0x%08x %s%s%s\n",bkpt_str[ptr_list->type],ptr_list->bk_count,address,
        cause!=NULL?"(":"",cause!=NULL?cause:"",cause!=NULL?")":"");
}

void del(BKPT_LIST * ptr_list,uint32_t address)
{
    BKPT * ptr=ptr_list->head;

    if(!ptr_list->head)
    {
        printf("%s bkpt list is empty\n",bkpt_str[ptr_list->type]);
        return;
    }

    if(ptr_list->head->address == address)
    {
        ptr_list->head=ptr_list->head->nxt;
        printf("Removed %s bkpt for 0x%08x %s%s%s\n",
            bkpt_str[ptr_list->type],address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",ptr->cause!=NULL?")":"");
        delete ptr;
        ptr_list->bk_count--;
        updateFctPointer(ptr_list);
        return;
    }

    while(ptr->nxt && ptr->nxt->address != address)
        ptr=ptr->nxt;
    if(!ptr->nxt)
        printf("Didn't find %s bkpt for 0x%08x\n",bkpt_str[ptr_list->type],address);
    else
    {
        BKPT * ptr2=ptr->nxt;
        printf("Removed %s bkpt for 0x%08x %s%s%s\n",
            bkpt_str[ptr_list->type],address,ptr->nxt->cause!=NULL?"(":"",ptr->nxt->cause!=NULL?ptr->nxt->cause:"",
            ptr->nxt->cause!=NULL?")":"");
        ptr->nxt = ptr->nxt->nxt;
        delete ptr2;
        ptr_list->bk_count--;
        updateFctPointer(ptr_list);
    }
}

void updateFctPointer(BKPT_LIST * ptr_list)
{
    if(ptr_list->bk_count == 0)
        ptr_list->fct = fct_void;
    else
        ptr_list->fct = has_bkpt;
}

void fct_void(BKPT_LIST * ptr_list,uint32_t address,int mode)
{
    
}

void has_bkpt(BKPT_LIST * ptr_list,uint32_t address,int mode)
{
    BKPT * ptr=ptr_list->head;

    while(ptr && ptr->address <= address)
    {
        if(address >= ptr->address && address <= (ptr->address+ptr->size))
        {
            switch(ptr_list->type)
            {
                case BKPT_MEM:
                    printf("%s %s BREAKPOINT at 0x%08x %s%s%s\n",bkpt_str[ptr_list->type],mode==BKPT_MEM_READ?"read":"write",
                        address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",
                        ptr->cause!=NULL?")":"");
                    break;
                case BKPT_CPU:
                    printf("%s BREAKPOINT at 0x%08x %s%s%s\n",bkpt_str[ptr_list->type],
                        address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",
                        ptr->cause!=NULL?")":"");
                    //exit(0);
                    break;
            }    
            CHG_RUN_MODE(STEP);
            return ;
        }
        ptr=ptr->nxt;
    }
}

void print_bkpt_list(bkpt_list * ptr_list)
{
    printf("%s Breakpoint list",bkpt_str[ptr_list->type]);
    if(ptr_list->head)
    {
        printf(":\n");
        for(BKPT * ptr=ptr_list->head;ptr!=NULL;ptr=ptr->nxt)
        {
            printf("0x%08x %s%s%s\n",ptr->address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",ptr->cause!=NULL?")":"");
        }
    }
    else
    {
        printf(" empty\n");
    }
}
