/* 
*   include/time.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __TIME_H
#define __TIME_H

struct med_tm {
    int tm_ms;
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_wday;
    int tm_mday;
    int tm_mon;
    int tm_year;
};

#endif
