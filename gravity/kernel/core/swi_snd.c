/* 
*   kernel/swi_snd.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>

__IRAM_CODE int swi_snd_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
        case nAPI_MIXER:
            mixer_ctl((int)nParam1,(int)nParam2,(void *)nParam3);
            return 0;
	}

	return 0;
}
