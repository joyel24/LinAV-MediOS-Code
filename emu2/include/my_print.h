/* 
*   my_print.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MY_PRINT_H
#define __MY_PRINT_H

#include <emu.h>
#include <mem_space.h>

#define print_data(TYPE,DATA,LENGTH)  if((HW_mode & TYPE) || run_mode==STEP) my_print_data(DATA,LENGTH);

void my_print_data(char * data,int length);
void my_print_mem(mem_space * mem,char * data,int length);

#endif /* __MY_PRINT_H */
