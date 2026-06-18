/*
*   include/kernel/preview.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __PREVIEW_H
#define __PREVIEW_H

#if defined(DSC25) || defined(DM270)
    #define PREVIEW_BASE               0x00030780
    #include <kernel/preview_DM270.h>
#elif defined(DM320)
    #define PREVIEW_BASE               0x00030A80
    #include <kernel/preview_DM320.h>
#elif defined(DSC21)
    #define PREVIEW_BASE               0x00030800
    #warning DSC21 probably needs its own defines
    #include <kernel/preview_DM270.h>
#else
    #error NO CHIP defined
#endif

#endif
