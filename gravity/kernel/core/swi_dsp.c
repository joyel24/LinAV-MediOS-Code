/* 
*   kernel/swi_dsp.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/threads.h>
#include <kernel/dsp.h>
#include <stdarg.h>

int swi_dsp_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
	case nAPI_DSP_OPEN:             //(void* pHandler);
	{
		// lock dsp mutex here, and setup dsp irq handler
	}
	break;

	case nAPI_DSP_CLOSE:            //();
	{
		// unlock dsp mutex here, and clear dsp irq handler
	}
	break;

	case nAPI_DSP_LOAD_MEMCODE:     //(void* pCode, int nSize);
	{
//		load_dsp_program_mem ((void*)nParam1, nParam2);
	}
	break;

	case nAPI_DSP_LOAD_HDDCODE:     //(const char* pszCoffProgram);
	{
		load_dsp_program ((const char*)nParam1);
	}
	break;

	case nAPI_DSP_ON:               //();
	{
		dsp_on ();
	}
	break;

	case nAPI_DSP_OFF:              //();
	{
		dsp_off ();
	}
	break;

	case nAPI_DSP_RESET:            //();
	{
		dsp_reset ();
	}
	break;

	case nAPI_DSP_RUN:              //();
	{
		dsp_run ();
	}
	break;

	default:
		return ERR_NOT_IMPLEMENTED;
	}

	return 0;
}
