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
#include <kernel/kgraphics.h>

GFX_Z_RECT* g_pZRectList = 0;

extern char screen_VID1 [SCR_WIDTH * SCR_HEIGHT *4 + 40];
extern char screen_VID2 [320 * 240 *4 + 40];

GFX_CONTEXT g_PhysicalScreen;

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

typedef struct _GFX_SPAN
{
	int x;
	int n;
	int w;
	unsigned long* p;
	struct _GFX_SPAN* pNext;
} GFX_SPAN;

#define SPAN_POOL_SIZE 1024

static GFX_SPAN** g_ppSpan [SCR_HEIGHT];
static MEMORY_CONTEXT g_SpanContext [SCR_HEIGHT];

static GFX_RECT g_Phys;

static unsigned char* g_pSpanPool = 0;

CRITSEC_INFO* g_pCS_GFX;

// Finds intersection of two rects (common area)
// Returns false, if intersection area is empty
__IRAM_CODE int GetRectIntersection (GFX_RECT* pRIn1, GFX_RECT* pRIn2, GFX_RECT* pROut)
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

__IRAM_CODE void GFX_DestroySpanStructure (int nYmin, int nYmax)
{
	int i;
	unsigned char* pSpanPoolPtr = g_pSpanPool + SPAN_POOL_SIZE * nYmin;
	for (i=nYmin;i<nYmax;i++)
	{
		// NOTE: Or we can simply destroy micro-pool...
		init_memory_context (g_SpanContext + i);
		bpool (g_SpanContext + i, pSpanPoolPtr, SPAN_POOL_SIZE);
		pSpanPoolPtr += SPAN_POOL_SIZE;

/*
		GFX_SPAN* pSpan = g_ppSpan[i];
		while (pSpan)
		{
			GFX_SPAN* pTmp = pSpan->pNext;
			brel (g_SpanContext + i, pSpan);
			pSpan = pTmp;
		}
*/

		g_ppSpan[i] = 0;
	}
}

__IRAM_CODE void GFX_BuildSpanStructure (int nYmin, int nYmax)
{
//	int i = 0;
	GFX_Z_RECT* pCtx = g_pZRectList;
	while (pCtx)//for (i=0;i<m_stack.size();i++)
	{
		int nX = pCtx->ptLocation.x;//m_stack[i].x;
		int nN = pCtx->pOwner->pMemoryContext->w;//m_stack[i].w;

		if (nX >= SCR_WIDTH)
			continue;

		if (nX + nN <= 0)
			continue;

		unsigned long* pP = pCtx->pOwner->pMemoryContext->pixels;//m_stack[i].p;
		unsigned long nStride = pCtx->pOwner->pMemoryContext->delta / 4;//m_stack[i].w;

		if (nX < 0)
		{
			nN += nX;
			pP -= nX;
			nX = 0;
		}

		if (nX + nN > SCR_WIDTH)
		{
			nN = SCR_WIDTH - nX;
		}

		int j;
		int stY = pCtx->ptLocation.y;
		int stH = pCtx->pOwner->pMemoryContext->h;
		for (j=stY;j<stH+stY;j++)
		{
			if ((j < nYmin) || (j >= nYmax))
			{
				pP += nStride;
				continue;
			}

			GFX_SPAN* pNewSpan = (GFX_SPAN*)bget (g_SpanContext + j, sizeof(GFX_SPAN));//new GFX_SPAN ();
			pNewSpan->x = nX;
			pNewSpan->n = nN;
			pNewSpan->w = pCtx->pOwner;
			pNewSpan->p = pP;
			pNewSpan->pNext = 0;

			if (!g_ppSpan[j])
			{// We are first !!
				g_ppSpan[j] = pNewSpan;
			}
			else
			{
				GFX_SPAN* pSpan = g_ppSpan[j];
				GFX_SPAN* pPrevSpan = 0;
				while (pSpan)
				{
					if (pSpan->x >= nX)
						break;
					pPrevSpan = pSpan;
					pSpan = pSpan->pNext;
				};

				if (!pPrevSpan)
				{// We are the first span...
					g_ppSpan[j] = pNewSpan;

					if (pSpan->x + pSpan->n > nX + nN)
					{// We can simply move old first span...
						int nShift = nN;
						pSpan->n -= nShift;
						pSpan->x += nShift;
						pSpan->p += nShift;
						pNewSpan->pNext = pSpan;
					}
					else
					{// We skip several invisible spans...
						while (1)
						{
							if (pSpan->x + pSpan->n <= pNewSpan->x + pNewSpan->n)
							{
								// delete span, go to next
								GFX_SPAN* pTmpSpan = pSpan;
								pSpan = pSpan->pNext;
								pTmpSpan->pNext = 0;
								brel (g_SpanContext + j, pTmpSpan);//delete pTmpSpan;
								if (!pSpan)
									break;
								continue;
							}
							else
							{
								int nShift = pNewSpan->x + pNewSpan->n - pSpan->x;
								pSpan->x += nShift;
								pSpan->p += nShift;
								pSpan->n -= nShift;
								pNewSpan->pNext = pSpan;
								break;
							}
						}
					}
				}
				else
				if (pPrevSpan->x + pPrevSpan->n > nX + nN)
				{// We are is inside one big span... Lets split them!
					GFX_SPAN* pTail = (GFX_SPAN*)bget (g_SpanContext + j, sizeof(GFX_SPAN));//new GFX_SPAN ();
					pTail->x = nX + nN;
					pTail->n = pPrevSpan->x + pPrevSpan->n - pTail->x;
					pTail->w = pPrevSpan->w;
					pTail->p = pPrevSpan->p + pTail->x - pPrevSpan->x;
					pTail->pNext = pSpan;
					pPrevSpan->pNext = pNewSpan;
					pNewSpan->pNext = pTail;
					pPrevSpan->n = nX - pPrevSpan->x;
				}
				else
				{// Masking the end of span...
					if (!pSpan)
					{// Simply add span at the end...
						pPrevSpan->n = nX - pPrevSpan->x;
						pPrevSpan->pNext = pNewSpan;
					}
					else
					{
						pPrevSpan->pNext = pNewSpan;
						pPrevSpan->n = pNewSpan->x - pPrevSpan->x;
						while (1)
						{
							if (pSpan->x + pSpan->n <= pNewSpan->x + pNewSpan->n)
							{
								// delete span, go to next
								GFX_SPAN* pTmpSpan = pSpan;
								pSpan = pSpan->pNext;
								pTmpSpan->pNext = 0;
								brel (g_SpanContext + j, pTmpSpan);//delete pTmpSpan;
								if (!pSpan)
									break;
								continue;
							}
							else
							{
								int nShift = pNewSpan->x + pNewSpan->n - pSpan->x;
								pSpan->x += nShift;
								pSpan->p += nShift;
								pSpan->n -= nShift;
								pNewSpan->pNext = pSpan;
								break;
							}
						}
					}
				}
			}
			pP += nStride;
		}
		pCtx = pCtx->pNext;
//		i ++;
	}
}

__IRAM_CODE void GFX_init ()
{
	printk ("GFX subsystem starting...\n");

	TASK_INFO* pTask = 0;
	API_TASK_GETHANDLE (&pTask);

	API_CRITSEC_CREATE (&g_pCS_GFX);

	g_PhysicalScreen.w = SCR_WIDTH;
	g_PhysicalScreen.h = SCR_HEIGHT;
	g_PhysicalScreen.x = 0;
	g_PhysicalScreen.y = 0;
	g_PhysicalScreen.bg_color = 0;
	g_PhysicalScreen.color = 0;
	g_PhysicalScreen.direction = 0;
	g_PhysicalScreen.pixel_size = 4;
	g_PhysicalScreen.pixels = screen_VID1;
	g_PhysicalScreen.delta = SCR_WIDTH*4;

	int i;

//	printk ("GFX [a]\n");
	API_MALLOC (&g_pSpanPool, SCR_HEIGHT * SPAN_POOL_SIZE);
//	printk ("GFX [b]\n");

	unsigned char* pSpanPoolPtr = g_pSpanPool;
	for (i=0;i<SCR_HEIGHT;i++)
	{
		g_ppSpan[i] = 0;
		init_memory_context (g_SpanContext + i);
		bpool (g_SpanContext + i, pSpanPoolPtr, SPAN_POOL_SIZE);
		pSpanPoolPtr += SPAN_POOL_SIZE;
	}

	g_Phys.x = 0;
	g_Phys.y = 0;
	g_Phys.w = SCR_WIDTH;
	g_Phys.h = SCR_HEIGHT;

	API_MALLOC (&pTask->pMemoryContext, sizeof(GFX_CONTEXT));
	pTask->pMemoryContext->w = SCR_WIDTH;
	pTask->pMemoryContext->h = SCR_HEIGHT;
	pTask->pMemoryContext->x = 0;
	pTask->pMemoryContext->y = 0;
	pTask->pMemoryContext->bg_color = 0;
	pTask->pMemoryContext->color = 0;
	pTask->pMemoryContext->direction = 0;
	pTask->pMemoryContext->pixel_size = 4;
	pTask->pMemoryContext->pixels = screen_VID2;
	pTask->pMemoryContext->delta = SCR_WIDTH*4;
	GFX_Z_RECT* pBackgroundZ = 0;
	API_MALLOC (&pBackgroundZ, sizeof(GFX_Z_RECT));
	pBackgroundZ->ptLocation.x = 0;
	pBackgroundZ->ptLocation.y = 0;
	pBackgroundZ->pPrev = 0;
	pBackgroundZ->pNext = 0;
	pBackgroundZ->pOwner = pTask;
	__cli();
	g_pZRectList = pBackgroundZ;
	__sti();
//	printk ("GFX Manager [2]\n");
	GFX_BuildSpanStructure (0, SCR_HEIGHT);
//	printk ("GFX Manager [3]\n");

	open_graphics ();

	hidePlane (BMAP1);
	hidePlane (BMAP2);
	hidePlane (VID1);
	hidePlane (VID2);
	hidePlane (CUR1);
	hidePlane (CUR2);

	showPlane (VID1);

	API_GFX_UPDATE_RECT (0);
	printk ("GFX subsystem initialized\n");
}

__IRAM_CODE void GFX_MoveContext (TASK_INFO* pOwner, int nX, int nY)
{
//printk ("GFX_MoveContext [0] (%i, %i)\n", nX, nY);

	API_CRITSEC_ENTER (g_pCS_GFX);

//API_TASK_SLEEP (1000);
//printk ("GFX_MoveContext [1] (%i, %i)\n", nX, nY);

	GFX_Z_RECT* pCtx = g_pZRectList;

	while (pCtx)
	{
		if (pCtx->pOwner == pOwner)
			break;
		pCtx = pCtx->pNext;
	}

	if (pCtx->pOwner == pOwner)
	{
		GFX_RECT rc1;
		rc1.x = pCtx->ptLocation.x;//m_stack[nWinID].x;
		rc1.y = pCtx->ptLocation.y;//m_stack[nWinID].y;
		rc1.w = pCtx->pOwner->pMemoryContext->w;//m_stack[nWinID].w;
		rc1.h = pCtx->pOwner->pMemoryContext->h;//m_stack[nWinID].h;

		GFX_RECT rc2;
		rc2.x = nX;
		rc2.y = nY;
		rc2.w = rc1.w;
		rc2.h = rc2.w;

		long nYmin = MAX(MIN(rc1.y, rc2.y), 0);
		long nYmax = MIN(MAX(rc1.y + rc1.h, rc2.y + rc2.h), SCR_HEIGHT);

//API_TASK_SLEEP (1000);
//printk ("GFX_MoveContext [2]\n");

		GFX_DestroySpanStructure (nYmin, nYmax);

		pCtx->ptLocation.x = nX;
		pCtx->ptLocation.y = nY;

//printk ("GFX_MoveContext [2] pCtx->ptLoc: (%i, %i)\n", nX, nY);

//API_TASK_SLEEP (1000);
//printk ("GFX_MoveContext [3]\n");

		GFX_BuildSpanStructure (nYmin, nYmax);

//GFX_DumpSpans ();

//API_TASK_SLEEP (1000);
//printk ("GFX_MoveContext [4]\n");

		GFX_RECT rcx;
		if (GetRectIntersection (&rc1, &rc2, &rcx))
		{// Redraw whole changed rect
			rcx.x = MIN(rc1.x, rc2.x);
			rcx.y = nYmin;
			rcx.w = MAX(rc1.x + rc1.w, rc2.x + rc2.w) - rcx.x;
			rcx.h = nYmax - rcx.y;
//printk ("GFX_MoveContext [4a] - before update...\n");
			GFX_UpdateRect (&rcx);
//printk ("GFX_MoveContext [4a] - after update...\n");
		}
		else
		{// Redraw two separated update rects
			GFX_UpdateRect (&rc1);
			GFX_UpdateRect (&rc2);
		}
	}

//API_TASK_SLEEP (1000);
//printk ("GFX_MoveContext [5]\n");

	API_CRITSEC_LEAVE (g_pCS_GFX);
}

__IRAM_CODE void GFX_UpdateRect (GFX_RECT* UpdateRect)
{
	API_CRITSEC_ENTER (g_pCS_GFX);

	if (UpdateRect)
	{
		if (!GetRectIntersection (UpdateRect, &g_Phys, UpdateRect))
		{
			API_CRITSEC_LEAVE (g_pCS_GFX);
			return;
		}
	}
	else
		UpdateRect = &g_Phys;

/*
printk ("GFX_UpdateRect UPD:(%d,%d,%d,%d)\n",
		UpdateRect->x,
		UpdateRect->y,
		UpdateRect->w,
		UpdateRect->h);
*/

	unsigned long* pOutput = (unsigned long*)g_PhysicalScreen.pixels + UpdateRect->x + UpdateRect->y * SCR_WIDTH;

//printk ("GFX_UpdateRect Output: %08X\n", pOutput);

	int i;
	for (i=0;i<UpdateRect->h;i++)
	{
		GFX_SPAN* pSpan = g_ppSpan[i + UpdateRect->y];

		while (pSpan)
		{
			if (pSpan->x + pSpan->n >= UpdateRect->x)
				break;
			pSpan = pSpan->pNext;
		};

		if (pSpan)
		{

			long x = UpdateRect->x;
			int nLast = 0;
			unsigned long* po = pOutput;
			unsigned long* p = pSpan->p + (x - pSpan->x);
			long n = pSpan->n - (x - pSpan->x);
			do
			{
				if (x + n >= UpdateRect->x + UpdateRect->w)
				{
					n = UpdateRect->w + UpdateRect->x - x;
					nLast = 1;
				}

				//int k;
				//for (k=0;k<n;k++)
				//	*po++ = *p++;
				//memcpy (po, p, n*4);
				//po += n; p += n;
				gfxmemcpy (po, p, n);
				po += n; p += n;

				x += n;
				pSpan = pSpan->pNext;
				if (!pSpan)
					break;
				p = pSpan->p;
				n = pSpan->n;
			}
			while (!nLast);

		}

		pOutput += SCR_WIDTH;
	}

	API_CRITSEC_LEAVE (g_pCS_GFX);
}

__IRAM_CODE void GFX_UpdateContext (TASK_INFO* pOwner, GFX_RECT* UpdateRect)
{
	API_CRITSEC_ENTER (g_pCS_GFX);

//printk ("GFX_UpdateContext [1] pRect: %d\n", UpdateRect);

	GFX_Z_RECT* pCtx = g_pZRectList;

	while (pCtx)
	{
		if (pCtx->pOwner == pOwner)
			break;
		pCtx = pCtx->pNext;
	}

	if (pCtx->pOwner == pOwner)
	{
		GFX_RECT ctx;
		ctx.x = pCtx->ptLocation.x;//m_stack[nWinID].x;
		ctx.y = pCtx->ptLocation.y;//m_stack[nWinID].y;
		ctx.w = pCtx->pOwner->pMemoryContext->w;//m_stack[nWinID].w;
		ctx.h = pCtx->pOwner->pMemoryContext->h;//m_stack[nWinID].h;

/*
printk ("GFX_UpdateContext [2] CTX:(%d,%d,%d,%d)\n",
		ctx.x,
		ctx.y,
		ctx.w,
		ctx.h);
*/

		if (UpdateRect)
		{
			if (!GetRectIntersection (UpdateRect, &ctx, UpdateRect))
			{
				API_CRITSEC_LEAVE (g_pCS_GFX);
				return;
			}
		}
		else
			UpdateRect = &ctx;

/*
printk ("GFX_UpdateContext [3] PHYS:(%d,%d,%d,%d)\n",
		g_Phys.x,
		g_Phys.y,
		g_Phys.w,
		g_Phys.h);

printk ("GFX_UpdateContext [4] UPD:(%d,%d,%d,%d)\n",
		UpdateRect->x,
		UpdateRect->y,
		UpdateRect->w,
		UpdateRect->h);
*/

		if (!GetRectIntersection (UpdateRect, &g_Phys, UpdateRect))
		{
			API_CRITSEC_LEAVE (g_pCS_GFX);
			return;
		}

//printk ("GFX_UpdateContext [5]\n");

		unsigned long* pOutput = (unsigned long*)g_PhysicalScreen.pixels + UpdateRect->y * SCR_WIDTH;

		int i;
		for (i=0;i<UpdateRect->h;i++)
		{
			GFX_SPAN* pSpan = g_ppSpan[i + UpdateRect->y];
			long nLast = 0;
			while (pSpan && (!nLast))
			{
				if (pSpan->x >= UpdateRect->x + UpdateRect->w)
					break;

				if (pSpan->w == pOwner/*nWinID*/)
				{

					unsigned long* po = pOutput + pSpan->x;
					unsigned long* p = pSpan->p;
					long n = pSpan->n;
					if (pSpan->x + n > UpdateRect->x + UpdateRect->w)
					{
						n = UpdateRect->x + UpdateRect->w - pSpan->x;
						nLast = 1;
					}

					//int k;
					//for (k=0;k<n;k++)
					//	*po++ = *p++;
					//memcpy (po, p, n*4);
					gfxmemcpy (po, p, n);
				}

				pSpan = pSpan->pNext;
			};

			pOutput += SCR_WIDTH;
		}
	}

	API_CRITSEC_LEAVE (g_pCS_GFX);
}

__IRAM_CODE void GFX_DumpSpans ()
{
	API_CRITSEC_ENTER (g_pCS_GFX);

	int i;
	for (i=0;i<SCR_HEIGHT;i+=8)
	{
		GFX_SPAN* pSpan = g_ppSpan[i];

		printk ("Raw %d: ", i);
		while (pSpan)
		{
			printk ("[%X->%X(%04d)]", pSpan->w, pSpan->p, pSpan->n);
			pSpan = pSpan->pNext;
		}

		printk ("\n");

		API_TASK_SLEEP (10);
	}

	API_CRITSEC_LEAVE (g_pCS_GFX);
}

__IRAM_CODE void GFX_AddContext (TASK_INFO* pOwner, int nX, int nY)
{
	API_CRITSEC_ENTER (g_pCS_GFX);

//API_TASK_SLEEP (1000);
//printk ("GFX_AddContext [1] (%i, %i)\n", nX, nY);

	GFX_Z_RECT* pCtx = g_pZRectList;

	while (pCtx->pNext)
		pCtx = pCtx->pNext;

	GFX_Z_RECT* pZRect = 0;
	API_MALLOC (&pZRect, sizeof(GFX_Z_RECT));

	pZRect->ptLocation.x = nX;
	pZRect->ptLocation.y = nY;
	pZRect->pOwner = pOwner;
	pZRect->pNext = 0;

	pCtx->pNext = pZRect;
	pZRect->pPrev = pCtx;

//API_TASK_SLEEP (1000);
//printk ("GFX_AddContext [2]\n");

	GFX_DestroySpanStructure (0, SCR_HEIGHT);

//API_TASK_SLEEP (1000);
//printk ("GFX_AddContext [3]\n");

	GFX_BuildSpanStructure (0, SCR_HEIGHT);

//API_TASK_SLEEP (1000);
//printk ("GFX_AddContext [4]\n");

//	GFX_DumpSpans ();

	API_CRITSEC_LEAVE (g_pCS_GFX);
}

void GFX_UpdateZOrder (TASK_INFO* pOwner, int nZOrder)
{
	API_CRITSEC_ENTER (g_pCS_GFX);

	// ...

	API_CRITSEC_LEAVE (g_pCS_GFX);
}
