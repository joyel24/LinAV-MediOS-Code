/* 
*   HW_ON_OFF.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either exEODess of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <gio_MAS_EOD.h>
#include <i2c_MAS.h>

#include <cmd_line.h>

gio_MAS_EOD * gio_MAS_EOD_obj;

int do_cmd_eod_s(int argc,char ** argv)
{ 
    return gio_MAS_EOD_obj->do_cmd_eod();    
}

int gio_MAS_EOD::do_cmd_eod(void)
{
    state = state==1?0:1;
    printf("New state: %d\n",state);
    return 0;
}

gio_MAS_EOD::gio_MAS_EOD(void):gpio_port(GPIO_MAS_EOD,"MAS_EOD")
{ 
    state = 1; 
    gio_MAS_EOD_obj = this;
    cnt = 0x30;
    cnt_loop=0;
    add_cmd_fct("eod",do_cmd_eod_s,"EOD");
}

void gio_MAS_EOD::chkEOD(void)
{
    if(cnt==0)
    {
        cnt_loop--;
        if(cnt_loop<=0)
        {
            state = 1;
            //printf("EOD 1\n");
            cnt=0xF0;
            cnt_loop=0;
        }
    }
}

void gio_MAS_EOD::set_gpio(void)
{
    printf("set EOD\n");
    state = 1;
}

void gio_MAS_EOD::clear_gpio(void)
{
    printf("clr EOD\n");
    state = 0;
}

void gio_MAS_EOD::gpio_dir_chg(int dir)
{
    printf("EOD dir changed to %d\n",dir);
}

