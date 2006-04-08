/*
*   include/kernel/swi.h
*
*   MediOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SWI_H
#define __SWI_H

struct user_fct_t {
    char * name;
    unsigned int ptr;
};

#define nAPI_INIAPI     0x1


    
#endif
