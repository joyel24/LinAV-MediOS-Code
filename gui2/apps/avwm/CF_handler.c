/*
* CF_handler.c
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
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mount.h>

#include "misc.h"
#include "events.h"
#include "avevents.h"
#include "plugin.h"

#include "CF_handler.h"

int cf_mnt_state=0;

struct plugin CF_mnt_plugin;
extern struct plugin cur_plugin;

int CF_is_mounted(void)
{
    return cf_mnt_state;
}

void mountCF(void)
{
    int res=0;
    if(CF_is_connected())
    {
        if(cf_mnt_state)
        {
            fprintf(stderr,"[mountCF] try to umount\n");
            res=umount2("/cf",MNT_FORCE);
        }
        if(res>=0)
        {
            fprintf(stderr,"[mountCF] mounting\n");
            do_fct(DO_MOUNT,"/dev/avcf1","/cf");
        }
        else
            fprintf(stderr,"[mountCF] no mounting as it is already mounted\n");
        cf_mnt_state=1;
    }
    else
        fprintf(stderr,"[mountCF] no mounting as it is already mounted\n");
    sendEvt(&cur_plugin,EVT_CF_ADDED);
}



void umountCF(void)
{
    int res;
    char pwd[10];
    sendEvt(&cur_plugin,EVT_CF_REMOVED);
    getcwd(pwd, 10);
    pwd[10]='\0';
    fprintf(stderr,"[UmountCF] pwd= %s\n",pwd);
    if(pwd[0]=='/' && pwd[1]=='c' && pwd[2]=='f' && (pwd[3]=='/'||pwd[3]=='\0'))    
    {
        fprintf(stderr,"[UmountCF] chg dir\n");
        chdir("/mnt");
    }    
    res=umount2("/cf",MNT_FORCE);
    fprintf(stderr,"[UmountCF] res=%d-err=%d\n",res,errno);
    //do_fct(DO_UMOUNT,"/dev/avcf1","/cf");
    if(res>=0)
        cf_mnt_state=0;
}

int do_fct(int fct,char * devFile,char * dirMnt)
{
   int pid;
   int status;
   
   pid = vfork();
   if (pid == 0)
   {
       if(fct==DO_MOUNT)
           execl("/bin/mount", "mount",devFile,dirMnt,(char*)NULL);
       else
           execl("/bin/umount", "umount",dirMnt,(char*)NULL);
       fprintf(stderr, "mount failed %s on %s\n",devFile,dirMnt);
       _exit(1);       
   }
   else
   {
       if(pid>0)
           waitpid(pid, &status, 0);
       else
       {
           fprintf(stderr, "vfork failed for mount %s on %s\n",devFile,dirMnt);
           return 0;
       }
   }
   return 1;
}

void CFmntEvtHandler(int evt)
{
    switch (evt)
    {
        case EVT_CF_IN:
            mountCF();            
            break;
        case EVT_CF_OUT:
            umountCF();            
            break;
    }          
}

void ini_CF_mount(void)
{
    mountCF();
    doRegisterPlugin(&CF_mnt_plugin,CFmntEvtHandler,0);
    CF_mnt_plugin.handle_on=1;
}
