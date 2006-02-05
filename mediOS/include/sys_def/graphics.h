/* 
*   include/sys_def/graphics.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GRAPHICS_STRUCT_H
#define __GRAPHICS_STRUCT_H

// vplane constant
#define BMAP1   0
#define BMAP2   1
#define VID1    2
#define VID2    3
#define CUR1    4
#define CUR2    5

struct spriteBuffer {
    unsigned int  data;
    int           width;
    int           height;
    int           type;
    int           bpline;
};

typedef struct spriteBuffer       SPRITE;
typedef struct spriteBuffer       BITMAP;

// sprite buffer type
#define S1bit      0
#define S2bit      1
#define S4bit      2
#define S8bit      3
#define S16bit     4
#define S24bit     5
#define S32bit     6

#define PALETTE unsigned int 

#endif
