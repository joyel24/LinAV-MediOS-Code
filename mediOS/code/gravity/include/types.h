/* 
*   include/types.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __TYPES_H
#define __TYPES_H

// API data type

#include <kernel/graphics_struct.h>

typedef struct _GFX_CONTEXT
{
	long x;
	long y;
	long w;
	long h;
	int direction;
	long delta;
	unsigned int color;
	unsigned int bg_color;
	long pixel_size;
	void* pixels;
} GFX_CONTEXT;

typedef struct _GFX_POINT
{
	long x;
	long y;
} GFX_POINT;

typedef struct _GFX_RECT
{
	long w;
	long h;
	long x;
	long y;
} GFX_RECT;

typedef struct _GFX_3DVERTEX
{
	long ix, iy, iz, iu, iv;
} GFX_3DVERTEX;

typedef struct _GFX_BLENDPARAMS
{
	long x;
	long y;
	long mode;
	long transparency;
} GFX_BLENDPARAMS;

typedef struct _GFX_DATA
{
    long x;
    long y;
    long w;
    long h;
    int direction;
    long delta; 
    unsigned int color;
    unsigned int bg_color;
} GFX_DATA;

typedef struct _PLANE_DATA
{
    int vplane;
    int state;
    int bpp;
} PLANE_DATA;

typedef struct _COUPLE_DATA
{
    void * a;
    void * b;
} COUPLE_DATA;

typedef unsigned long HTASK;
typedef unsigned long HPIPE;
typedef unsigned long HCRITSEC;
typedef unsigned long HEAP;
typedef unsigned long HFONT;

typedef unsigned long COLOR;

typedef struct _MESSAGE
{
	unsigned long nMsg;
	unsigned long nParam1;
	unsigned long nParam2;
} MESSAGE;

typedef enum _E_Z_ORDER
{
	eTOPMOST = 0,
	eFOREGROUND = 1
} E_Z_ORDER;

#endif
