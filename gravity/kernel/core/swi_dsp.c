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
#include <kernel/irq.h>

#include <api.h>

#include <stdarg.h>

static HCRITSEC g_hDSPLock = 0;

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
		if (g_hDSPLock == 0)
			API_CRITSEC_CREATE (&g_hDSPLock);

		if (API_CRITSEC_TRYENTER (g_hDSPLock) != ERR_OK)
			return ERR_DSP_BUSY;

		g_pDSPHandler = (DSP_HANDLER)nParam1;
		enable_irq (IRQ_DSP);
	}
	break;

	case nAPI_DSP_CLOSE:            //();
	{
		// unlock dsp mutex here, and clear dsp irq handler
		dsp_off ();
		disable_irq (IRQ_DSP);
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	case nAPI_DSP_LOAD_MEMCODE:     //(void* pCode, int nSize);
	{
		API_CRITSEC_ENTER (g_hDSPLock);
		load_dsp_program_mem ((void*)nParam1, nParam2);
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	case nAPI_DSP_LOAD_HDDCODE:     //(const char* pszCoffProgram);
	{
		API_CRITSEC_ENTER (g_hDSPLock);
		load_dsp_program_hdd ((const char*)nParam1);
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	case nAPI_DSP_ON:               //();
	{
		API_CRITSEC_ENTER (g_hDSPLock);
		dsp_on ();
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	case nAPI_DSP_OFF:              //();
	{
		API_CRITSEC_ENTER (g_hDSPLock);
		dsp_off ();
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	case nAPI_DSP_RESET:            //();
	{
		API_CRITSEC_ENTER (g_hDSPLock);
		dsp_reset ();
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	case nAPI_DSP_RUN:              //();
	{
		API_CRITSEC_ENTER (g_hDSPLock);
		dsp_run ();
		API_CRITSEC_LEAVE (g_hDSPLock);
	}
	break;

	default:
		return ERR_NOT_IMPLEMENTED;
	}

	return 0;
}
