/* 
*   kernel/swi_snd.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
    
    case nAPI_SOUND_BUFFER_WRITE:            
//            api_param->count=sound_buff_write((sound_buffer_s *) nParam1, api_param->reader_fct,api_param->count,(void*) nParam3);
        return 0;
    
    case nAPI_MIXER:
        mixer_ctl((int)nParam1,(int)nParam2,(void *)nParam3);
        return 0;
    case nAPI_DSP:
        dsp_ctl((int)nParam1,(void *)nParam2);
        return 0;
