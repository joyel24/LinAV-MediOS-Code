/* 
*   kernel/swi_snd.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>

#include <kernel/sound.h>


__IRAM_CODE int swi_snd_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
    sound_api_param * api_param = (sound_api_param*)nParam2;
    switch (nCmd)
    {
        case nAPI_SOUND_BUFFER_WRITE:            
            api_param->count=sound_buff_write((sound_buffer_s *) nParam1, api_param->reader_fct,api_param->count,(void*) nParam3);
            return 0;
        
        case nAPI_MIXER:
            mixer_ctl((int)nParam1,(int)nParam2,(void *)nParam3);
            return 0;
        case nAPI_DSP:
            dsp_ctl((int)nParam1,(void *)nParam2);
            return 0;
    }
    return 0;
}
