/* 
*   include/kernel/kfont.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __KFONT_H
#define __KFONT_H

#include <sys_def/font.h>

#define needFont(name)     extern FONT_ID name;

#define NBFONT        0x16

void ini_font(void);

typedef enum _E_FONT_TYPE
{
	eFNT_2BIT_PACK  = 0,
	eFNT_8BIT_MONO  = 1,
	eFNT_8BIT_VARY  = 2,
	eFNT_32BIT_MONO = 3,
	eFNT_32BIT_VARY = 4,
} E_FONT_TYPE;

typedef struct _FONT_HEADER
{
	char cSignature [4];
	E_FONT_TYPE nFontType;
	char cSymbolMap [256];
} FONT_HEADER;

typedef struct _FONT_2BIT_PACK_HEADER
{
	long nSymWidth;
	long nSymHeight;
} FONT_2BIT_PACK_HEADER;

typedef struct _FONT_8BIT_MONO_HEADER
{
	long nSymWidth;
	long nSymHeight;
} FONT_8BIT_MONO_HEADER;

typedef struct _FONT_8BIT_VARY_HEADER
{
	long nSymHeight;
	char abc [256][3];
} FONT_8BIT_VARY_HEADER;

typedef struct _FONT_32BIT_MONO_HEADER
{
	long nSymWidth;
	long nSymHeight;
} FONT_32BIT_MONO_HEADER;

typedef struct _FONT_32BIT_VARY_HEADER
{
	long nSymHeight;
	char abc [256][3];
} FONT_32BIT_VARY_HEADER;

#endif
