/*
* plugin.c
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

#include <stdio.h>
#include <errno.h>

#include "avevents.h"
#include "cops.h"
#include "plugin.h"

extern struct client_operations cops;

struct plugin cur_plugin={
    handler:NULL,
    pid:-1,
    handle_on:0,
    flag:0
};

struct plugin status_bar_plugin={
    handler:NULL,
    pid:-1,
    handle_on:0,
    flag:0
};

struct plugin menu_plugin={
    handler:NULL,
    pid:-1,
    handle_on:0,
    flag:0
};

void registerPlugin(void (*evtHandle),int flag)
{
	doRegisterPlugin(&cur_plugin,evtHandle,flag);
}

void doRegisterPlugin(struct plugin * plug,void (*evtHandle),int flag)
{
	plug->handler=evtHandle;
        plug->pid=-1;
        plug->flag=flag;
}

void stop_me(void)
{
    cur_plugin.handler=0;
    showSBar();
    menu_plugin.handle_on=1;
    enableMenu();
    sendEvt(&menu_plugin,EVT_REDRAW);       
}

int loadPlugin(char * path, char * param)
{
    int status;
    fprintf(stderr,"load plug: %s\n",path);
    if(param!=NULL)
    	fprintf(stderr,"Param=%s\n",param);
    if(cur_plugin.handler)
    {
        cur_plugin.handler(EVT_RESUME);
        cur_plugin.handler(EVT_QUIT);
        cur_plugin.handler=NULL;
        waitpid(cur_plugin.pid, &status, 0);
        cur_plugin.pid=-1;
    }
    drawGui();
    return launchPlugin(path,param);
}

int launchPlugin(char * path,char * param)
{
    int pid,err;
    char cops_a[15];
    
    sprintf(cops_a, "%d",(int)&cops);
    pid = vfork();
    if (pid == 0)
    { // child
        if(param!=NULL)
            err = execl(path, path,param,cops_a,(char *)0);
        else
            err = execl(path, path,cops_a,(char *)0);
        fprintf(stderr, "exec failed!%s %d %ld\n", path, err, errno);
        _exit(1);
    }
    else {
        if (pid < 0) {
            fprintf(stderr, "vfork failed %d\n", pid);
            return -1;
        }
        cur_plugin.pid=pid;
        fprintf(stderr,"load success pid= %d\n",pid);
    }
    return 0;

}

extern struct wkUP_evt nxt_action;

int playMp3(char * filename)
{
	fprintf(stderr,"[playMP3] name:%s\n",filename);
        nxt_action.app=APP_MP3;
        strcpy(nxt_action.arg,filename);
	wakeUP();
}