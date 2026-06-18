/* 
*   include/kernel/cf_module.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CF_MODULE_H
#define __CF_MODULE_H

#include <kernel/ext_module.h>

void cf_initModule(void);

#define CF_IS_CONNECTED (get_connected_module()==AV_MODULE_CFC?1:0)

#endif
