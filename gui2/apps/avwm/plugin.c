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
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "avevents.h"
#include "events.h"
#include "cops.h"
#include "plugin.h"
#include "status_line.h"
#include "avstring.h"
#include "avwm-menu.h"
#include "avgraphics.h"

#ifndef AV_SCREEN
    #include <dlfcn.h>
#endif

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

struct plugin settings_plugin={
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

struct plugin msg_box_plugin={
    handler:NULL,
    pid:-1,
    handle_on:0,
    flag:0
};

struct plugin edit_box_plugin={
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
    /* setting font back to normal */
    defaultFont();
    defaultPlane();
    drawGui();
    return launchPlugin(path,param);
}

int launchPlugin(char * path,char * param)
{
    char cops_a[15];
#ifdef AV_SCREEN
    int pid,err,status;
#else
    void* pd;
    int (*main_start)(int argc,char ** argv);
    char * argv[2];
#endif
    
    sprintf(cops_a, "%d",(int)&cops);
    
#ifdef AV_SCREEN
    pid = vfork();
    if (pid == 0)
    { // child
        if(param!=NULL)
            err = execl(path, path,param,cops_a,(char *)NULL);
        else
            err = execl(path, path,cops_a,(char *)NULL);
        fprintf(stderr, "exec failed!%s %d %d\n", path, err, errno);
        _exit(1);
    }
    else {
        if (pid < 0) {
            fprintf(stderr, "vfork failed %d\n", pid);
            return -1;
        }
        cur_plugin.pid=pid;        
        menu_plugin.handle_on=0;
        fprintf(stderr,"load success pid= %d\n",pid);
        waitpid(pid, &status, 0);        
        fprintf(stderr,"[launchPlugin] after wait\n",pid);
    }    
    return 0;
#else
    pd = dlopen(path, RTLD_NOW);
    if (!pd) {
        fprintf(stderr,"Can't open %s with dlopen: %s\n",path,dlerror());        
        dlclose(pd);
        return -1;
    }    
    
    main_start = dlsym(pd, "main");    
    if (!main_start) {
        fprintf(stderr,"Can't find main: %s\n",dlerror());
        dlclose(pd);
        return -1;
    }
    
    menu_plugin.handle_on=0;
    
    argv[0]=path;
    argv[1]=cops_a;
    main_start(2,argv);
    return 0;
#endif
}

extern struct wkUP_evt nxt_action;

int playMp3(char * filename)
{
    fprintf(stderr,"[playMP3] name:%s\n",filename);
    nxt_action.app=APP_MP3;
    strcpy(nxt_action.arg,filename);
    wakeUP();
    return 1;
}
