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

enum
{
	eGFX_MGR_NOP,        // Simply test...
	eGFX_MGR_ADD,        // Add context in Z-list
	eGFX_MGR_DELETE,     // Remove context from Z-list
	eGFX_MGR_FOREGROUND, // Sets context as foreground context
	eGFX_MGR_MOVE,       // Moves graphics context to another screen position
	eGFX_MGR_COMMIT,     // Commit changes in given context
} E_GFX_MGR_CMD;

#endif
