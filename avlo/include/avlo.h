/*
*   include/avlo.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __AVLO_H_
#define __AVLO_H_

/*



void drawProgress(int offset,int length,int mode);
void drawBox(void);*/

void chkOFF(int key);
void drawMenu(int nb);
void drawBox(int txt_width,int txt_height,int * start_x,int * start_y);
void waitKeyReleased(int has_time_out);
void err(int i);
void affUSB(void);
void moveCursor(int direction);
int  processDefault(int key,int nbCfg);
void printErr(int key);
int fastLoadCJBM(char * filename);
int loadFile(char * fileN,char* buffer,int prog);

#endif
