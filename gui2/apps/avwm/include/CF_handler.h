/*
* CF_handler.h
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

#ifndef __CF_HANDLER_H
#define __CF_HANDLER_H

#define DO_MOUNT    0
#define DO_UMOUNT   1

void mountCF           (void);
void umountCF          (void);
int  do_fct            (int fct,char * devFile,char * dirMnt);
void CFmntEvtHandler   (int evt);
void ini_CF_mount      (void);

#endif
