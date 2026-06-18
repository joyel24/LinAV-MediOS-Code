/*
* include/kernel/cfg_file.h
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __CFG_FILE_H
#define __CFG_FILE_H

#include <sys_def/types.h>

void cfg_clear();

void cfg_newFile();
bool cfg_readFile(char * filename);
bool cfg_writeFile(char * filename);

void cfg_rewindItems();
bool cfg_nextItem(char * * name,char * * value);

bool cfg_itemExists(char * name);

char * cfg_readString(char * name);
int cfg_readInt(char * name);
bool cfg_readBool(char * name);

void cfg_writeString(char * name,char * value);
void cfg_writeInt(char * name,int value);
void cfg_writeBool(char * name,bool value);

void cfg_addDummyLine(char * text);

bool cfg_deleteItem(char * name);

// debug
void cfg_printItems();

#endif
