/*
* misc.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __MISC_H
#define __MISC_H

#include "av3xx_common.h"

int   getTimeS          (char * timeSt);
int   getTime           (struct tm * date_time);

int   set_mouseParam    (int freq, int repeat);

int   getBat            (void);

int   getPwr            (void);

int   getUSB            (void);
int   setUSB            (int state);

#endif