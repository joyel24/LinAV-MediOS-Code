/*
* file_handle_fct.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either eXpress of implied.
*
*/

#ifndef __FILE_HANDLE_FCT_H
#define __FILE_HANDLE_FCT_H

int  do_mv     (char * src,char * dest);
int  do_cp     (char * src,char * dest);
int  execBin   (char * path, ...);

/** Private functions **/
int   isadir     (char *name);
char* buildname  (char * dirname,char * filename);
int   copyfile   (char * src,char * dest,int setmodes);


#endif

