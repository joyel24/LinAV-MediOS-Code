/* 
*   kernel/swi_gfx.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/threads.h>
#include <stdarg.h>

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

__IRAM_CODE int swi_gfx_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
	case nAPI_GFX_CREATE_CONTEXT:   //(int nWidth, int nHeight, int nFlags);
	{
		TASK_INFO* pTask = 0;
		__cli ();
		pTask = g_pTaskRing;
		__sti ();

		GFX_CONTEXT* pCtx = 0;
		API_MALLOC (&pCtx, sizeof(GFX_CONTEXT));

		pCtx->w = nParam1;
		pCtx->h = nParam2;
		pCtx->x = 0;
		pCtx->y = 0;
		pCtx->direction = 0;
		pCtx->delta = pCtx->w * 4;
		pCtx->pixel_size = 4;

		API_MALLOC (&pCtx->pixels, pCtx->w * pCtx->h * 4);

		pTask->pMemoryContext = pCtx;

//		kgfx_manager_handler (eGFX_MGR_ADD, 0, 0, pTask);
		GFX_AddContext (pTask, 0, 0);

/*
		__cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pGFXManagerPipe->buffer + g_pGFXManagerPipe->nSender);
		pSysCtrl->nCmdId = eGFX_MGR_ADD;
		pSysCtrl->nCmdParam1 = 0;//Context's X
		pSysCtrl->nCmdParam2 = 0;//Context's Y
		pSysCtrl->pSenderThread = g_pTaskRing;
		g_pGFXManagerPipe->nSender = (g_pGFXManagerPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		/// Block calling task...
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_GFXMGR;
		g_pTaskRing->nBlockingValue = 0;

		API_TASK_YIELD ();
		__sti ();
*/
	}
	break;

	case nAPI_GFX_UPDATE_RECT:       //(GFX_RECT* pArea);
	{
		TASK_INFO* pTask = 0;
		__cli ();
		pTask = g_pTaskRing;
		__sti ();
//		kgfx_manager_handler (eGFX_MGR_COMMIT, nParam1, 0, pTask);
		GFX_RECT* pRect = (GFX_RECT*)nParam1;

//printk ("Before GFX_UpdateContext: pRect = %d, \n", pRect, g_pZRectList->pOwnerTask);

		GFX_UpdateContext (pTask, pRect);

/*
		__cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pGFXManagerPipe->buffer + g_pGFXManagerPipe->nSender);
		pSysCtrl->nCmdId = eGFX_MGR_COMMIT;
		pSysCtrl->nCmdParam1 = nParam1;
		pSysCtrl->nCmdParam2 = nParam2;
		pSysCtrl->pSenderThread = g_pTaskRing;
		g_pGFXManagerPipe->nSender = (g_pGFXManagerPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		/// Block calling task...
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_GFXMGR;
		g_pTaskRing->nBlockingValue = 0;

		API_TASK_YIELD ();
		__sti ();
*/
	}
	break;

	case nAPI_GFX_MOVE:         //(GFX_POINT* pOrigin)                                            { swi_call(nAPI_GFX_MOVE); }
	{
		TASK_INFO* pTask = 0;
		__cli ();
		pTask = g_pTaskRing;
		__sti ();
//		kgfx_manager_handler (eGFX_MGR_MOVE, nParam1, 0, pTask);
		GFX_MoveContext (pTask, ((GFX_POINT*)nParam1)->x, ((GFX_POINT*)nParam1)->y);

/*
		__cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pGFXManagerPipe->buffer + g_pGFXManagerPipe->nSender);
		pSysCtrl->nCmdId = eGFX_MGR_MOVE;
		pSysCtrl->nCmdParam1 = nParam1;
		pSysCtrl->pSenderThread = g_pTaskRing;
		g_pGFXManagerPipe->nSender = (g_pGFXManagerPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		/// Block calling task...
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_GFXMGR;
		g_pTaskRing->nBlockingValue = 0;

		API_TASK_YIELD ();
		__sti ();
*/
	}
	break;

	case nAPI_GFX_FOREGROUND:   //()                                                              { swi_call(nAPI_GFX_FOREGROUND); }
	{
		TASK_INFO* pTask = 0;
		__cli ();
		pTask = g_pTaskRing;
		__sti ();
//		kgfx_manager_handler (eGFX_MGR_FOREGROUND, nParam1, 0, pTask);
		GFX_UpdateZOrder (pTask, 0);

/*
		__cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pGFXManagerPipe->buffer + g_pGFXManagerPipe->nSender);
		pSysCtrl->nCmdId = eGFX_MGR_FOREGROUND;
		pSysCtrl->pSenderThread = g_pTaskRing;
		g_pGFXManagerPipe->nSender = (g_pGFXManagerPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		/// Block calling task...
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_GFXMGR;
		g_pTaskRing->nBlockingValue = 0;

		API_TASK_YIELD ();
		__sti ();
*/
	}
	break;

	case nAPI_GFX_FASTBLIT:     //(GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_POINT* pOrigin);
	{
		GFX_CONTEXT* pDst     = (GFX_CONTEXT*)nParam1;
		GFX_CONTEXT* pSrc     = (GFX_CONTEXT*)nParam2;
		GFX_POINT* pOrigin = (GFX_POINT*)nParam3;

//		printk ("blitting [1]...\n");

		unsigned char nSrcElementSize = pSrc->pixel_size;
		unsigned char nDstElementSize = pDst->pixel_size;

		int bDoReverse = 0;
		if (pSrc->direction != pDst->direction)
			bDoReverse = 1;

//		printk ("dw=%d, dh=%d, sw=%d, sh=%d\n", pDst->w, pDst->h, pSrc->w, pSrc->h);

		int xmin = MAX(pOrigin->x,0);
		int ymin = MAX(pOrigin->y,0);
		int xmax = MIN(pDst->w, pSrc->w + pOrigin->x);
		int ymax = MIN(pDst->h, pSrc->h + pOrigin->y);
		int dx = xmax - xmin;
		int dy = ymax - ymin;

//		printk ("xmin=%d, ymin=%d, xmax=%d, ymax=%d\n", xmin, ymin, xmax, ymax);

		if ((!dx) || (!dy))
			break;

//		printk ("blitting [2]...\n");

		int i,j;

		long st = pSrc->delta;
		long dt = pDst->delta;
		unsigned char* sptr;
		unsigned char* dptr = (unsigned char*)(pDst->pixels + dt*ymin + xmin*nDstElementSize);

		if (bDoReverse)
		{
			if (pOrigin->y < 0)
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pOrigin->x)*nSrcElementSize + (dy-1)*st);
			else
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pOrigin->x)*nSrcElementSize + (pSrc->h-1)*st);
			st = -st;
		}
		else
		{
			sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pOrigin->x)*nSrcElementSize + MAX(0,-pOrigin->y)*st);
		}

//		printk ("blitting [3]...\n");

		if (nSrcElementSize == nDstElementSize)
		{
//			printk ("blitting [4]...\n");
//			int nRow = dx * nDstElementSize;
			for (i=0;i<dy;i++)
			{
//				memcpy (dptr, sptr, nRow);
				gfxmemcpy (dptr, sptr, dx);
				dptr += dt;
				sptr += st;
			}
		}
		else
			return ERR_INVALID_PARAM;
	}
	break;

	case nAPI_GFX_BLENDBLIT:     //(GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_BLENDPARAMS* pParams);
	{
		GFX_CONTEXT* pDst           = (GFX_CONTEXT*)nParam1;
		GFX_CONTEXT* pSrc           = (GFX_CONTEXT*)nParam2;
		GFX_BLENDPARAMS* pParams = (GFX_POINT*)nParam3;

		unsigned char nSrcElementSize = pSrc->pixel_size;
		unsigned char nDstElementSize = pDst->pixel_size;

		int bDoReverse = 0;
		if (pSrc->direction != pDst->direction)
			bDoReverse = 1;

//		printk ("dw=%d, dh=%d, sw=%d, sh=%d\n", pDst->w, pDst->h, pSrc->w, pSrc->h);

		int xmin = MAX(pParams->x,0);
		int ymin = MAX(pParams->y,0);
		int xmax = MIN(pDst->w, pSrc->w + pParams->x);
		int ymax = MIN(pDst->h, pSrc->h + pParams->y);
		int dx = xmax - xmin;
		int dy = ymax - ymin;

//		printk ("xmin=%d, ymin=%d, xmax=%d, ymax=%d\n", xmin, ymin, xmax, ymax);

		if ((!dx) || (!dy))
			break;

//		printk ("blitting [2]...\n");

		int i,j;

		long st = pSrc->delta;
		long dt = pDst->delta;
		unsigned char* sptr;
		unsigned char* dptr = (unsigned char*)(pDst->pixels + dt*ymin + xmin*nDstElementSize);

		if (bDoReverse)
		{
			if (pParams->y < 0)
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pParams->x)*nSrcElementSize + (dy-1)*st);
			else
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pParams->x)*nSrcElementSize + (pSrc->h-1)*st);
			st = -st;
		}
		else
		{
			sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pParams->x)*nSrcElementSize + MAX(0,-pParams->y)*st);
		}

//		printk ("blitting [3]...\n");

//		if ((nSrcElementSize == nDstElementSize) && (nSrcElementSize == 4))
		if (nSrcElementSize == nDstElementSize)
		{
//			printk ("blitting [4]...\n");
			int nRow = dx * nDstElementSize;
			unsigned long nC1 = pParams->transparency;
			unsigned long nC2 = 256 - nC1;
//			unsigned long nMix;
			for (i=0;i<dy;i++)
			{
//				memcpy (dptr, sptr, nRow);
				for (j=0;j<nRow;j++)
				{
					dptr[j] = (sptr[j] * nC1 + dptr[j] * nC2) >> 8;
				}
				dptr += dt;
				sptr += st;
			}
		}
		else
			return ERR_INVALID_PARAM;
	}
	break;

	case nAPI_GFX_STRETCHBLIT: //GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, unsigned long* pWorkBuffer
	{
		GFX_CONTEXT* pDst = (GFX_CONTEXT*)nParam1;
		GFX_CONTEXT* pSrc = (GFX_CONTEXT*)nParam2;

		if ((pSrc->pixel_size != 4) || (pDst->pixel_size != 4))
			return ERR_INVALID_PARAM;

		AlphaBilinearInterpolatedStretch (
			pDst,
			pSrc,
			(unsigned long*)nParam3,
			(unsigned long*)nParam3 + pDst->w);
	}
	break;

	case nAPI_GFX_PATTERNBLIT: //(GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc);
	{
		GFX_CONTEXT* pDst    = (GFX_CONTEXT*)nParam1;
		GFX_CONTEXT* pSrc    = (GFX_CONTEXT*)nParam2;

		GFX_POINT org;
		for (org.y=0;org.y<pDst->h+pSrc->h;org.y+=pSrc->h)
		for (org.x=0;org.x<pDst->w+pSrc->w;org.x+=pSrc->w)
			kcswi_handler (nAPI_GFX_FASTBLIT, nParam2, (unsigned long)&org, nParam1);
	}
	break;

/// TMP !!!

	case nAPI_GFX:
	{
		printk ("nAPI_GFX call...\n");

		TASK_INFO* pTask = 0;
		__cli ();
		pTask = g_pTaskRing;
		__sti ();

		if (!pTask->pMemoryContext)
			return ERR_NO_GFX_CONTEXT;

		return gfx_swi_handler (
			pTask->pMemoryContext,
			(int)nParam1,
			(GFX_CONTEXT*)nParam2,
			(void*)nParam3);
	}
	break;

        case nAPI_PRINTF:
            user_printf((const char *)nParam1, (va_list) nParam2);
            return 0;

/// TMP !!!
	}

	return 0;
}
