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

CRITSEC_INFO* g_pCS_GFX;

void kgfx_init ()
{
	API_CRITSEC_CREATE (&g_pCS_GFX);

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
}

// GFX manager
// This thread serializes calls to commit changes on common screen area,
// Made from diffrent threads.
//__IRAM_CODE int kgfx_manager (void* pvParameters)
void kgfx_manager_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	TASK_INFO* pCallingTask)
{
//	printk ("[GFXMGR]: kgfx_manager_handler\n");
	API_CRITSEC_ENTER (g_pCS_GFX);
//	printk ("[GFXMGR]: switch %d...\n", nCmd);

	switch (nCmd)
	{
		case eGFX_MGR_NOP:
			printk ("[GFXMGR]: eGFX_MGR_NOP\n");
			break;

		case eGFX_MGR_ADD:
		{
			printk ("[GFXMGR]: eGFX_MGR_ADD\n");

			GFX_Z_RECT* pZRect = 0;
			API_MALLOC (&pZRect, sizeof(GFX_Z_RECT));
			__cli();

			pZRect->ptLocation.x = nParam1;
			pZRect->ptLocation.y = nParam2;
			pZRect->pOwner = pCallingTask;
			pZRect->pNext = 0;

			GFX_Z_RECT* pTopmost = g_pZRectList;
			while (pTopmost->pNext)
			{
				pTopmost = pTopmost->pNext;
			};

			pZRect->pPrev = pTopmost;
			pTopmost->pNext = pZRect;
			__sti();
		}
		break;

		case eGFX_MGR_DELETE:
			printk ("[GFXMGR]: eGFX_MGR_DELETE\n");
			break;

		case eGFX_MGR_FOREGROUND:
			printk ("[GFXMGR]: eGFX_MGR_FOREGROUND\n");
			break;

		case eGFX_MGR_MOVE:
		{
/* MOVE ALGORITHM
// 1. Determine Z-Order position of quering context
// 2. Find one/two rects of move-from area
// 3. Updates all stack for move-from area
// 4. IF context area is not interleaved with any of above contexts,
//         Put it directly into physical plane, set "no back" flag in Z-struct ???
//    ELSE
//         Redraw all above contexts, intersected with new context position area
*/

//			printk ("[GFXMGR]: eGFX_MGR_MOVE start\n");
			GFX_Z_RECT* pCurWindow = g_pZRectList;
			GFX_DATA SubContext;

			// 1.
			while (pCurWindow)
			{
				if (pCurWindow->pOwner == pCallingTask)
					break;

				pCurWindow = pCurWindow->pNext;
			};

			if (pCurWindow)
			{
				GFX_RECT OldContextRect;
				OldContextRect.x = 0;
				OldContextRect.y = 0;
				OldContextRect.w = pCurWindow->pOwner->pMemoryContext->w;
				OldContextRect.h = pCurWindow->pOwner->pMemoryContext->h;
				TranslateRectIntoPhysicalSpace (&OldContextRect, pCurWindow);

				GFX_Z_RECT* pWorkWindow = g_pZRectList;
				while (pWorkWindow)
				{
					if (pWorkWindow->pOwner == pCallingTask)
						break;

					GetSubContext (&SubContext, &g_CompilingBuffer, &OldContextRect);

//					printk ("BLIT MEM -> COMPILE [ERASE]\n");
					API_GFX_FASTBLIT (
						&g_CompilingBuffer,
						pWorkWindow->pOwner->pMemoryContext,
						&pWorkWindow->ptLocation);

					pWorkWindow = pWorkWindow->pNext;
				};

				GetSubContext (&SubContext, &g_PhysicalScreen, &OldContextRect);
				GFX_POINT pt;
				pt.x = 0;
				pt.y = 0;
//				printk ("BLIT COMPILE -> PHYS [ERASE]\n");
				API_GFX_FASTBLIT (
					&g_PhysicalScreen,
					&g_CompilingBuffer,
					&pt);

				pCurWindow->ptLocation.x = ((GFX_POINT*)nParam1)->x;
				pCurWindow->ptLocation.y = ((GFX_POINT*)nParam1)->y;

				GFX_RECT NewContextRect;
				NewContextRect.x = 0;
				NewContextRect.y = 0;
				NewContextRect.w = pCurWindow->pOwner->pMemoryContext->w;
				NewContextRect.h = pCurWindow->pOwner->pMemoryContext->h;
				TranslateRectIntoPhysicalSpace (&NewContextRect, pCurWindow);

				while (pWorkWindow)
				{
					GetSubContext (&SubContext, &g_CompilingBuffer, &NewContextRect);

//					printk ("BLIT MEM -> COMPILE [DRAW]\n");
					API_GFX_FASTBLIT (
						&g_CompilingBuffer,
						pWorkWindow->pOwner->pMemoryContext,
						&pWorkWindow->ptLocation);

					pWorkWindow = pWorkWindow->pNext;
				}

				GetSubContext (&SubContext, &g_PhysicalScreen, &NewContextRect);
//				printk ("BLIT COMPILE -> PHYS [DRAW]\n");
				API_GFX_FASTBLIT (
					&g_PhysicalScreen,
					&g_CompilingBuffer,
					&pt);
			}

//			printk ("[GFXMGR]: eGFX_MGR_MOVE end\n");
		}
		break;

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
					GFX_DATA SubContext;

					// 1.
					while (pCurWindow)
					{
						if (pCurWindow->pOwner == pCallingTask)
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

						if (nParam1)
							UpdateRect = *((GFX_RECT*)nParam1);
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

		case eGFX_MGR_REDRAW:
		{
			printk ("[GFXMGR]: eGFX_MGR_REDRAW\n");
		}
		break;

		default:
			printk ("[GFXMGR]: *** UNKNOWN *** (%d)\n", nCmd);
		break;
	}

	API_CRITSEC_LEAVE (g_pCS_GFX);

/*
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

		/// Unblock calling task...
		__cli ();
		pGfxCtrl->pSenderThread->nBlockingState = TASK_BLOCKED_BY_NONE;
		pGfxCtrl->pSenderThread->nBlockingValue = 0;
		API_TASK_YIELD ();
		__sti ();
	};

	return 0;
*/
}
