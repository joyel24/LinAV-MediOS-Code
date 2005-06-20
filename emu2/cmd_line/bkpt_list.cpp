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
    head=NULL;
}

void bkpt_list::add(uint32_t address)
{
    BKPT * ptr;
    BKPT * ptr_new = new BKPT();
    ptr_new->address = address;
    
    if(head == NULL || head->address > address) /* list empty or address < => insert at the beg*/
    {
        ptr_new->nxt = NULL;
        head = ptr_new;
    }
    else                                        /* let's find where to insert */
    {
        ptr=head;
        while(ptr->nxt!=NULL && ptr->nxt->address<address)
            ptr=ptr->nxt;
                
        ptr_new->nxt = ptr->nxt;
        ptr->nxt = ptr_new;        
    }
    
    printf("adding bkpt for 0x%08x\n",address);
    print_bkpt_list();
}

void bkpt_list::del(uint32_t address)
{
    BKPT * ptr=head;
    
    if(head->address == address)
    {
        head=head->nxt;
        printf("Removed bkpt for 0x%08x\n",address);
        return;
    }
    
    while(ptr->nxt && ptr->nxt->address != address)
        ptr=ptr->nxt;
    if(!ptr->nxt)
        printf("Didn't find bkpt for 0x%08x\n",address);
    else
    {
        BKPT * ptr2=ptr->nxt;
        ptr->nxt = ptr->nxt->nxt;
        printf("Removed bkpt for 0x%08x\n",address);
    }
}

bool bkpt_list::has_bkpt(uint32_t address)
{
    BKPT * ptr=head;

    while(ptr && ptr->address <= address)
    {
        if(ptr->address == address)
        {
            printf("BREAKPOINT at 0x%08x\n",ptr->address);
            return true;
        }
        ptr=ptr->nxt;
    }
    return false;
}

void bkpt_list::print_bkpt_list(void)
{
    printf("Breakpoint list:\n");
    for(BKPT * ptr=head;ptr!=NULL;ptr=ptr->nxt)
    {
        printf("0x%08x\n",ptr->address);
    }
}
