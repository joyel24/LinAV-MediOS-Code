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
#include <bkpt_list.h>

bkpt_list::bkpt_list(void)
{
    head[0]=NULL;
    head[1]=NULL;
    
    bkpt_str[0] = "CPU";
    bkpt_str[1] = "MEM";
}

void bkpt_list::add(uint32_t address,int type,char * cause)
{
    add(address,0,type,cause);
}

void bkpt_list::add(uint32_t address,int type)
{
    add(address,0,type,NULL);
}

void bkpt_list::add(uint32_t address,uint32_t size,int type)
{
    add(address,size,type,NULL);
}

void bkpt_list::add(uint32_t address,uint32_t size,int type,char * cause)
{
    BKPT * ptr;
    BKPT * ptr_new = new BKPT();
    ptr_new->address = address;
    ptr_new->size = size;
    ptr_new->cause = cause;
    
    if(head[type] == NULL || head[type]->address > address) /* list empty or address < => insert at the beg*/
    {
        ptr_new->nxt = NULL;
        head[type] = ptr_new;
    }
    else                                        /* let's find where to insert */
    {
        ptr=head[type];
        while(ptr->nxt!=NULL && ptr->nxt->address<address)
            ptr=ptr->nxt;
                
        ptr_new->nxt = ptr->nxt;
        ptr->nxt = ptr_new;        
    }
    
    printf("adding %s bkpt for 0x%08x %s%s%s\n",bkpt_str[type],address,cause!=NULL?"(":"",cause!=NULL?cause:"",cause!=NULL?")":"");
}

void bkpt_list::del(uint32_t address,int type)
{
    BKPT * ptr=head[type];
    
    if(head[type]->address == address)
    {
        head[type]=head[type]->nxt;
        printf("Removed %s bkpt for 0x%08x %s%s%s\n",
            bkpt_str[type],address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",ptr->cause!=NULL?")":"");
        delete ptr;
        return;
    }
    
    while(ptr->nxt && ptr->nxt->address != address)
        ptr=ptr->nxt;
    if(!ptr->nxt)
        printf("Didn't find %s bkpt for 0x%08x\n",bkpt_str[type],address);
    else
    {
        BKPT * ptr2=ptr->nxt;
        printf("Removed %s bkpt for 0x%08x %s%s%s\n",
            bkpt_str[type],address,ptr->nxt->cause!=NULL?"(":"",ptr->nxt->cause!=NULL?ptr->nxt->cause:"",ptr->nxt->cause!=NULL?")":"");
        ptr->nxt = ptr->nxt->nxt;        
        delete ptr2;
    }
}

bool bkpt_list::has_bkpt(uint32_t address,int type)
{
    BKPT * ptr=head[type];

    while(ptr && ptr->address <= address)
    {
        if(address >= ptr->address && address <= (ptr->address+ptr->size))
        {
            printf("%s BREAKPOINT at 0x%08x %s%s%s\n",
                bkpt_str[type],address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",ptr->cause!=NULL?")":"");
            return true;
        }
        ptr=ptr->nxt;
    }
    return false;
}

void bkpt_list::print_bkpt_list(int type)
{
    printf("%s Breakpoint list",bkpt_str[type]);
    if(head)
    {
        printf(":\n");
        for(BKPT * ptr=head[type];ptr!=NULL;ptr=ptr->nxt)
        {
            printf("0x%08x %s%s%s\n",ptr->address,ptr->cause!=NULL?"(":"",ptr->cause!=NULL?ptr->cause:"",ptr->cause!=NULL?")":"");
        }
    }
    else
    {
        printf(" empty\n");
    }
}
