/* 
*   include/sys_def/stddef.h
*
*   MedisOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#ifndef __SYS_DEF_STDDEF_H
#define __SYS_DEF_STDDEF_H

#define NULL ((void*)0)

#define abs(x) ((x)>0?(x):-(x))

void exit(int retcode);

#endif
