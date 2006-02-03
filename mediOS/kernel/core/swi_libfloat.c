/*
*   kernel/core/swi_libfloat.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/swi.h>

extern long __modsi3(long a, long b);
extern long __umodsi3(long a, long b);
extern long __divsi3(long a, long b);
extern long __udivsi3(long a, long b);


int swi_float_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
            case nAPI_MODSI3:        
                *(long*)nParam3=__modsi3(nParam1,nParam2);
                return MED_OK;
            case nAPI_DIVSI3:
                *(long*)nParam3=__divsi3(nParam1,nParam2);
                return MED_OK;
            case nAPI_UMODSI3:
                *(long*)nParam3=__umodsi3(nParam1,nParam2);
                return MED_OK;
            case nAPI_UDIVSI3:
                *(long*)nParam3=__udivsi3(nParam1,nParam2);
                return MED_OK;        
            default:
                printk("Device undefined libfloat swi (%d)\n",nCmd);
        }
        return 0;
}

