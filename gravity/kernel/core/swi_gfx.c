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

#include <sys_def/colordef.h>

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

__IRAM_CODE TASK_INFO* lock_task ()
{
		TASK_INFO* pTask = 0;
		__cli ();
		pTask = g_pTaskRing;
		__sti ();
		return pTask;
}

__IRAM_CODE int swi_gfx_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
	case nAPI_GFX_OPEN_GRAPHICS:    //();
	{
	}
	break;

	case nAPI_GFX_CLOSE_GRAPHICS:   //();
	{
	}
	break;

	case nAPI_GFX_CREATE_CONTEXT:   //(int nWidth, int nHeight, int nFlags);
	{
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
		memset (pCtx->pixels, 0, pCtx->w * pCtx->h * 4);

		lock_task()->pMemoryContext = pCtx;

//		kgfx_manager_handler (eGFX_MGR_ADD, 0, 0, pTask);
		GFX_AddContext (lock_task(), 0, 0);

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

	case nAPI_GFX_GET_CONTEXT:      //(GFX_CONTEXT* pCtx);
	{
		__cli ();
		if (g_pTaskRing->pMemoryContext)
			*((GFX_CONTEXT*)nParam1) = *g_pTaskRing->pMemoryContext;
		__sti ();
	}
	break;

	case nAPI_GFX_UPDATE_RECT:       //(GFX_RECT* pArea);
	{
		GFX_UpdateContext (lock_task(), (GFX_RECT*)nParam1);

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
//		kgfx_manager_handler (eGFX_MGR_MOVE, nParam1, 0, pTask);
		GFX_MoveContext (lock_task(), ((GFX_POINT*)nParam1)->x, ((GFX_POINT*)nParam1)->y);

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
//		kgfx_manager_handler (eGFX_MGR_FOREGROUND, nParam1, 0, pTask);
		GFX_UpdateZOrder (lock_task(), 0);

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

	case nAPI_GFX_CHARBLIT:     //(GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_POINT* pt);
	{
		TASK_INFO* pTask = lock_task ();

		GFX_CONTEXT* pDst = (GFX_CONTEXT*)nParam1;
		GFX_CONTEXT* pSrc = (GFX_CONTEXT*)nParam2;
		GFX_POINT* ptOrig = (GFX_POINT*)nParam3;

		unsigned char nSrcElementSize = pSrc->pixel_size;
		unsigned char nDstElementSize = pDst->pixel_size;

		int bDoReverse = 0;
		if (pSrc->direction != pDst->direction)
			bDoReverse = 1;

//		printk ("dw=%d, dh=%d, sw=%d, sh=%d\n", pDst->w, pDst->h, pSrc->w, pSrc->h);

		int xmin = MAX(ptOrig->x,0);
		int ymin = MAX(ptOrig->y,0);
		int xmax = MIN(pDst->w, pSrc->w + ptOrig->x);
		int ymax = MIN(pDst->h, pSrc->h + ptOrig->y);
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
			if (ptOrig->y < 0)
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-ptOrig->x)*nSrcElementSize + (dy-1)*st);
			else
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-ptOrig->x)*nSrcElementSize + (pSrc->h-1)*st);
			st = -st;
		}
		else
		{
			sptr = (unsigned char*)(pSrc->pixels + MAX(0,-ptOrig->x)*nSrcElementSize + MAX(0,-ptOrig->y)*st);
		}

		unsigned long nForeColor = pTask->nFontColor;//0x80808080;
		unsigned char* pForeColor = (unsigned char*)&nForeColor;

		if ((nSrcElementSize == 1) && (nDstElementSize == 4))
		{
			int nRow = dx * nDstElementSize;
			for (i=0;i<dy;i++)
			{
				for (j=0;j<nRow;j+=4)
				{
					unsigned long nC1 = sptr[j/4] + 1;
					unsigned long nC2 = 256 - nC1;
					dptr[j+0] = (dptr[j+0] * nC1 + pForeColor[0] * nC2) >> 8;
					dptr[j+1] = (dptr[j+1] * nC1 + pForeColor[1] * nC2) >> 8;
					dptr[j+2] = (dptr[j+2] * nC1 + pForeColor[2] * nC2) >> 8;
					dptr[j+3] = (dptr[j+3] * nC1 + pForeColor[3] * nC2) >> 8;
				}
				dptr += dt;
				sptr += st;
			}
		}
		else
			return ERR_INVALID_PARAM;
	}
	break;

	case nAPI_GFX_DRAWPIXEL:        //(int nX, int nY, COLOR Color);
	{
		GFX_CONTEXT* pCtx = lock_task ()->pMemoryContext;
		if (pCtx)
		{
			graphics32_DrawPixel (nParam3, nParam1, nParam2, pCtx);
		}
	}
	break;

	case nAPI_GFX_READPIXEL:        //(int nX, int nY);
	{
		GFX_CONTEXT* pCtx = lock_task ()->pMemoryContext;
		if (pCtx)
			return graphics32_ReadPixel (nParam1, nParam2, pCtx);
		else
			return 0;
	}
	break;

	case nAPI_GFX_DRAWLINE:         //(GFX_POINT* pt1, GFX_POINT* pt2, COLOR Color);
	{
		GFX_CONTEXT* pCtx = lock_task ()->pMemoryContext;
		if (pCtx)
		{
			GFX_POINT* pt1 = (GFX_POINT*)nParam1;
			GFX_POINT* pt2 = (GFX_POINT*)nParam2;
//			graphics32_DrawLine (nParam3, pt1->x, pt1->y, pt2->x, pt2->y, pCtx);
			graphics32_DrawAALine (pCtx, pt1->x, pt1->y, pt2->x, pt2->y, 255, nParam3, 1);
		}
	}
	break;

	case nAPI_GFX_DRAWRECT:         //(GFX_RECT* pRect, COLOR Color);
	{
		GFX_CONTEXT* pCtx = lock_task ()->pMemoryContext;
		if (pCtx)
		{
			GFX_RECT* pRect = (GFX_RECT*)nParam1;
			graphics32_DrawRect (nParam2, pRect->x, pRect->y, pRect->w, pRect->h, pCtx);
		}
	}
	break;

	case nAPI_GFX_FILLRECT:         //(GFX_RECT* pRect, COLOR Color);
	{
		GFX_CONTEXT* pCtx = lock_task ()->pMemoryContext;
		if (pCtx)
		{
			GFX_RECT* pRect = (GFX_RECT*)nParam1;
			graphics32_FillRect (nParam2, pRect->x, pRect->y, pRect->w, pRect->h, pCtx);
		}
	}
	break;

	case nAPI_GFX_SET_DRAWING_RECT: //(GFX_RECT* pRect);
	{
	}
	break;

	case nAPI_GFX_GET_DRAWING_RECT: //(GFX_RECT* pRect);
	{
	}
	break;

	case nAPI_PRINTF:
	{
		user_printf((const char *)nParam1, (va_list) nParam2);
	}
	break;

	case nAPI_SET_FONT:             //(HFONT hFont);
	{
		__cli ();
		g_pTaskRing->pFont = nParam1;
		__sti ();
	}
	break;

	case nAPI_GET_FONT:             //(HFONT* phFont);
	{
	}
	break;

	case nAPI_TEXT:                 //(const char* pszText, int x, int y);
	{
		printk ("We are inside nAPI_TEXT...\n");

		TASK_INFO* pTask = lock_task ();
		if (!pTask->pFont)
			return ERR_NO_FONT;

		if (pTask->pFont->nFontType != eFNT_8BIT_VARY)
			return ERR_INVALID_HANDLE;

		unsigned char* pCharMap = ((unsigned char*)pTask->pFont) + sizeof(FONT_HEADER);
		FONT_32BIT_VARY_HEADER* pVarMap = pCharMap + 256;

		char* text = (char*)nParam1;
		int x = nParam2;
		int y = nParam3;

		GFX_POINT char_pos;
		GFX_CONTEXT char_ctx;

		while (*text)
		{
			unsigned long nCharCode = *text;
			unsigned char nSym = pCharMap[nCharCode];

//			printk ("Printing symbol '%d' (%d x %d)...\n", nSym, pVarMap[nSym].w, pVarMap[nSym].h);

//			canvas.Blit (&pChars[nSym].sprite, x + pChars[nSym].A - pChars[nSym].Xo, y - pChars[nSym].Yo);
			char_pos.x = x + pVarMap[nSym].a;// - pVarMap[nSym].x;
			char_pos.y = y - pVarMap[nSym].h + pTask->pFont->nSymHeight;// - pVarMap[nSym].y;

			char_ctx.x = 0;
			char_ctx.y = 0;
			char_ctx.w = pVarMap[nSym].w;
			char_ctx.h = pVarMap[nSym].h;
			char_ctx.direction = 1;
			char_ctx.pixel_size = 1;
			char_ctx.pixels = ((unsigned char*)pTask->pFont) + pVarMap[nSym].offset;
			char_ctx.delta = pVarMap[nSym].w * 1;
			swi_gfx_handler (nAPI_GFX_CHARBLIT, pTask->pMemoryContext, &char_ctx, &char_pos);

			x += pVarMap[nSym].a + pVarMap[nSym].b + pVarMap[nSym].c;

			text ++;
		}

/*
		if (pTask->pFont->nFontType != eFNT_32BIT_VARY)
			return ERR_INVALID_HANDLE;

		unsigned char* pCharMap = ((unsigned char*)pTask->pFont) + sizeof(FONT_HEADER);
		FONT_32BIT_VARY_HEADER* pVarMap = pCharMap + 256;

		char* text = (char*)nParam1;
		int x = nParam2;
		int y = nParam3;

		GFX_POINT char_pos;
		GFX_CONTEXT char_ctx;

		while (*text)
		{
			unsigned long nCharCode = *text;
			unsigned char nSym = pCharMap[nCharCode];

//			printk ("Printing symbol '%d' (%d x %d)...\n", nSym, pVarMap[nSym].w, pVarMap[nSym].h);

//			canvas.Blit (&pChars[nSym].sprite, x + pChars[nSym].A - pChars[nSym].Xo, y - pChars[nSym].Yo);
			char_pos.x = x + pVarMap[nSym].a;// - pVarMap[nSym].x;
			char_pos.y = y - pVarMap[nSym].h + pTask->pFont->nSymHeight;// - pVarMap[nSym].y;

			char_ctx.x = 0;
			char_ctx.y = 0;
			char_ctx.w = pVarMap[nSym].w;
			char_ctx.h = pVarMap[nSym].h;
			char_ctx.direction = 1;
			char_ctx.pixel_size = 4;
			char_ctx.pixels = ((unsigned char*)pTask->pFont) + pVarMap[nSym].offset;
			char_ctx.delta = pVarMap[nSym].w * 4;
			swi_gfx_handler (nAPI_GFX_FASTBLIT, pTask->pMemoryContext, &char_ctx, &char_pos);

			x += pVarMap[nSym].a + pVarMap[nSym].b + pVarMap[nSym].c;

			text ++;
		}
*/
	}
	break;

	case nAPI_SET_FONT_COLOR:       //(COLOR nColor)                                                     { swi_call(nAPI_SET_FONT_COLOR); }
	{
		lock_task ()->nFontColor = nParam1;
	}
	break;

	case nAPI_GET_FONT_COLOR:       //(COLOR* pnColor)                                                   { swi_call(nAPI_GET_FONT_COLOR); }
	{
		__cli ();
		*((COLOR*)nParam1) = g_pTaskRing->nFontColor;
		__sti ();
	}
	break;

	case nAPI_GET_TEXT_RECT: //(const char* pszText, GFX_RECT* pRect);
	{
		TASK_INFO* pTask = lock_task ();
		if (!pTask->pFont)
			return ERR_NO_FONT;
		else
		{
		}
	}
	break;

/// TMP !!!

	case nAPI_GFX:
	{
		printk ("nAPI_GFX call...\n");

		TASK_INFO* pTask = lock_task ();

		if (!pTask->pMemoryContext)
			return ERR_NO_GFX_CONTEXT;

		return gfx_swi_handler (
			pTask->pMemoryContext,
			(int)nParam1,
			(GFX_CONTEXT*)nParam2,
			(void*)nParam3);
	}
	break;

/// TMP !!!

	case nAPI_GFX_BUILD_SPANS:   //(int nYmin, int nYmax);
	{
		GFX_BuildSpanStructure (nParam1, nParam2);
	}
	break;

	default:
		return ERR_NOT_IMPLEMENTED;
	}

	return 0;
}
