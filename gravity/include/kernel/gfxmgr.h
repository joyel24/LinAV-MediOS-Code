/* 
*   include/gfxmgr.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __GFXMGR_H
#define __GFXMGR_H

#include <kernel/threads.h>

typedef struct _GFX_Z_RECT
{
	TASK_INFO*          pOwner;
	GFX_POINT           ptLocation;
	struct _GFX_Z_RECT* pNext;
	struct _GFX_Z_RECT* pPrev;
} GFX_Z_RECT;

extern GFX_Z_RECT* g_pZRectList;

#define SCR_WIDTH  320
#define SCR_HEIGHT 240

void GFX_init ();
void GFX_BuildSpanStructure   (int nYmin, int nYmax);
void GFX_DestroySpanStructure (int nYmin, int nYmax);
int  GetRectIntersection      (GFX_RECT* pRIn1, GFX_RECT* pRIn2, GFX_RECT* pROut);
void GFX_MoveContext          (TASK_INFO* pOwner, int nX, int nY);
void GFX_UpdateRect           (GFX_RECT* UpdateRect);
void GFX_UpdateContext        (TASK_INFO* pOwner, GFX_RECT* UpdateRect);
void GFX_AddContext           (TASK_INFO* pOwner, int nX, int nY);
void GFX_UpdateZOrder         (TASK_INFO* pOwner, int nZOrder);

#endif
