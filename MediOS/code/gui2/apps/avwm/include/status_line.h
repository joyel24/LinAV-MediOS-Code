/*
* status_line.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Goetz Minuth
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __STATUS_LINE_H
#define __STATUS_LINE_H

void drawTime          (void);
void drawBat           (void);
void drawStatus        (void);
void drawLogo          (void);
void drawGui           (void);
int  statusEvtHandler  (int evt);
void ini_status_bar    (struct plugin * status_plugin);

void showSBar          (void);
void hideSBar          (void);
int  sBarStatus        (void);

#endif
