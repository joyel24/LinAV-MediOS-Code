/*
* plugin.h
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

#ifndef __PLUGIN_H
#define __PLUGIN_H

struct plugin {
	void (*handler)(int evt);
        int handle_on;
        int pid;
        int flag;
};



void   doRegisterPlugin (struct plugin * plug,void (*evtHandle),int flag);
void   registerPlugin   (void (*evtHandle),int flag);
int    loadPlugin       (char * path, char * param);  // prepare env to load a new plugin
int    launchPlugin     (char * path,char * param);   // launch a plugin


#endif