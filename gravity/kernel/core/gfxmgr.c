/* 
*   kernel/gfxmgr.c
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>
#include <kernel/malloc.h>

#include <kernel/kernel.h>
#include <kernel/threads.h>
#include <kernel/pipes.h>
#include <kernel/irq.h>
#include <kernel/swi.h>
#include <api.h>
#include <kernel/gfxmgr.h>

GFX_Z_RECT* g_pZRectList = 0;

extern char screen_VID1 [320 * 240 *4 + 40];
GFX_DATA g_PhysicalScreen;
GFX_DATA g_CompilingBuffer;

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

void TranslateRectIntoPhysicalSpace (GFX_RECT* pRect, GFX_Z_RECT* pDesc)
{
	pRect->x += pDesc->ptLocation.x;
	pRect->y += pDesc->ptLocation.y;
}

// Returns false, if intersection area is empty
int GetRectIntersection (GFX_RECT* pRIn1, GFX_RECT* pRIn2, GFX_RECT* pROut)
{
	int xmin = MAX(pRIn1->x, pRIn2->x);
	int ymin = MAX(pRIn1->y, pRIn2->y);

	int xmax = MIN(pRIn1->x + pRIn1->w, pRIn2->x + pRIn2->w);
	int ymax = MIN(pRIn1->y + pRIn1->h, pRIn2->y + pRIn2->h);

	int dx = xmax - xmin;
	int dy = ymax - ymin;

	if ((dx <= 0) || (dy <= 0))
		return 0;

	pROut->x = xmin;
	pROut->y = ymin;
	pROut->w = dx;
	pROut->h = dy;
	return 1;
}

// Assumes, that rect always is inside context-defined area
void GetSubContext (GFX_DATA* pSubContext, GFX_DATA* pContext, GFX_RECT* pRect)
{
	pSubContext->delta      = pContext->delta;
	pSubContext->direction  = pContext->direction;
	pSubContext->w          = pRect->w;
	pSubContext->h          = pRect->h;
	pSubContext->pixel_size = pContext->pixel_size;
	pSubContext->pixels     = pContext->pixels + pRect->x * pContext->pixel_size + pRect->y * pContext->delta;
}

// GFX manager runs as separate thread
// This thread serializes calls to commit changes on common screen area,
// Made from diffrent threads.
//__IRAM_CODE int kgfx_manager (void* pvParameters)
int kgfx_manager (void* pvParameters)
{
	g_PhysicalScreen.w = 320;
	g_PhysicalScreen.h = 240;
	g_PhysicalScreen.x = 0;
	g_PhysicalScreen.y = 0;
	g_PhysicalScreen.bg_color = 0;
	g_PhysicalScreen.color = 0;
	g_PhysicalScreen.direction = 0;
	g_PhysicalScreen.pixel_size = 4;
	g_PhysicalScreen.pixels = screen_VID1;
	g_PhysicalScreen.delta = 320*4;

	g_CompilingBuffer.w = 320;
	g_CompilingBuffer.h = 240;
	g_CompilingBuffer.x = 0;
	g_CompilingBuffer.y = 0;
	g_CompilingBuffer.bg_color = 0;
	g_CompilingBuffer.color = 0;
	g_CompilingBuffer.direction = 0;
	g_CompilingBuffer.pixel_size = 4;
	g_CompilingBuffer.delta = 320*4;
	API_MALLOC (&g_CompilingBuffer.pixels, 320 * 240 * 4);

	SYSTEM_CTRL_COMMAND* pGfxCtrl = 0;
	TASK_INFO* pTCB = 0;

	GFX_DATA SubContext;

	while (1)
	{
		pGfxCtrl = 0;
//		API_PIPE_RECV ((HPIPE)g_pGFXManagerPipe, &GfxCtrl, sizeof(SYSTEM_CTRL_COMMAND));

		__cli ();
		if (g_pGFXManagerPipe->nReceiver != g_pGFXManagerPipe->nSender)
		{
			pGfxCtrl = (SYSTEM_CTRL_COMMAND*)(g_pGFXManagerPipe->buffer + g_pGFXManagerPipe->nReceiver);
			g_pGFXManagerPipe->nReceiver = (g_pGFXManagerPipe->nReceiver + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;
		}
		else
		{
			API_TASK_YIELD ();
			continue;
		}
		__sti ();

		if (!pGfxCtrl)
			continue;

		switch (pGfxCtrl->nCmdId)
		{
			case eGFX_MGR_NOP:
				printk ("[GFXMGR]: eGFX_MGR_NOP\n");
				break;

			case eGFX_MGR_ADD:
				printk ("[GFXMGR]: eGFX_MGR_ADD\n");
				break;

			case eGFX_MGR_DELETE:
				printk ("[GFXMGR]: eGFX_MGR_DELETE\n");
				break;

			case eGFX_MGR_FOREGROUND:
				printk ("[GFXMGR]: eGFX_MGR_FOREGROUND\n");
				break;

			case eGFX_MGR_MOVE:
				printk ("[GFXMGR]: eGFX_MGR_MOVE\n");
				break;

/* MOVE ALGORITHM
// ???
// ???
// ???
*/

			case eGFX_MGR_COMMIT:
				{

/* COMMIT ALGORITHM
// 1. Determine Z-Order position of quering context
// 2. Intersect context area with areas of all above contexts
// 3. IF context area is not interleaved with any of above contexts,
//         Put it directly into physical plane, set "no back" flag in Z-struct ???
//    ELSE
//         Redraw all above contexts, intersected with updated area
*/

//					printk ("[GFXMGR]: eGFX_MGR_COMMIT begin\n");
					GFX_Z_RECT* pCurWindow = g_pZRectList;

					// 1.
					while (pCurWindow)
					{
						if (pCurWindow->pOwner == pGfxCtrl->pSenderThread)
							break;

						pCurWindow = pCurWindow->pNext;
					};

					if (pCurWindow)
					{
						GFX_RECT ContextRect;
						GFX_RECT UpdateRect;

						ContextRect.x = 0;
						ContextRect.y = 0;
						ContextRect.w = pCurWindow->pOwner->pMemoryContext->w;
						ContextRect.h = pCurWindow->pOwner->pMemoryContext->h;

						if (pGfxCtrl->nCmdParam1)
							UpdateRect = *((GFX_RECT*)pGfxCtrl->nCmdParam1);
						else
							UpdateRect = ContextRect;

						TranslateRectIntoPhysicalSpace (&UpdateRect, pCurWindow);

/*
						printk ("Update area [%i,%i,%i,%i] (TASK: %08x)\n",
							UpdateRect.x,
							UpdateRect.y,
							UpdateRect.w,
							UpdateRect.h,
							pCurWindow->pOwner);
*/

						// 2-3.
						while (pCurWindow)
						{
							GetSubContext (&SubContext, &g_CompilingBuffer, &UpdateRect);

//							printk ("BLIT MEM -> COMPILE\n");
							API_GFX_FASTBLIT (
								&g_CompilingBuffer,
								pCurWindow->pOwner->pMemoryContext,
								&pCurWindow->ptLocation);

							pCurWindow = pCurWindow->pNext;
						}

						GetSubContext (&SubContext, &g_PhysicalScreen, &UpdateRect);
						GFX_POINT pt;
						pt.x = 0;
						pt.y = 0;
//						printk ("BLIT COMPILE -> PHYS\n");
						API_GFX_FASTBLIT (
							&g_PhysicalScreen,
							&g_CompilingBuffer,
							&pt);
					}

//					printk ("[GFXMGR]: eGFX_MGR_COMMIT end\n");
				}
				break;

			default:
				printk ("[GFXMGR]: *** UNKNOWN *** (%d)\n", pGfxCtrl->nCmdId);
				break;
		}

		/// Unblock calling task...
		__cli ();
		pGfxCtrl->pSenderThread->nBlockingState = TASK_BLOCKED_BY_NONE;
		pGfxCtrl->pSenderThread->nBlockingValue = 0;
		API_TASK_YIELD ();
		__sti ();
	};

	return 0;
}
