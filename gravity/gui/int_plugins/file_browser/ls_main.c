/*
* kernel/gui/internal_plugin/file_browser/ls_main.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <graphics.h>
#include <evt.h>
#include <api.h>
#include <sys_def/colordef.h>

//#include <gui/helperMenu.h>
#include <gui/file_browser.h>
#include <gui/file_type.h>



struct browser_data realData = {
    path            : "/",
    
    list            : NULL,
    listused        : 0,
    listsize        : 0,
    
    scroll_pos      : LEFT_SCROLL,
    
    pos             : 0,
    nselect         : 0,
   
    show_dot_files  : 1,

    nbFile          : 0,
    nbDir           : 0,
    totSize         : 0,
    
    nb_disp_entry   : 20,
    x_start         : 0,
    y_start         : 18,
    
    width           : 320,
    height          : 202,
    entry_height    : 10,
    
    draw_bottom_status : draw_bottom_status,
    draw_file_size     : draw_file_size,
    clear_status       : clear_status
};

struct browser_data * bdata;

void printList(struct browser_data * bdata,int val)
{
    int i;
    printf("%d: Files:%d Dir:%d Size:%d ListUsed:%d\n",val,bdata->nbFile,bdata->nbDir,bdata->totSize,bdata->listused);
    printf("liste @:%x\n",(int)bdata->list);
    for(i=0;i<bdata->listused;i++)
        printf("%d: (%s) %s\n",i,bdata->list[i].type==TYPE_FILE?"F":"D",bdata->list[i].name);
    printf("---------------------------- %d\n",val);
}

void stopLs(void)
{
    //hideHelper();
}

void bwseventHandler(int evt)
{
    int w = 0;
    int h = 10;
    char path[PATHLEN];  

    getStringS("M", &w, &h);

    
            //helperEvt(evt,BTN_JOY);
            evt=browserEvt(evt,bdata);
            switch(evt)
            {
                case BTN_RIGHT:
                    if(bdata->list[bdata->pos+bdata->nselect].type==TYPE_FILE)
                    {     
//                        browser_listener.enable=0;
                        sprintf(path,"%s%s",bdata->path,bdata->list[bdata->pos+bdata->nselect].name);
                        handle_type_other(path);                        
                    }
                    break;
                case BTN_F1:
#warning need to code action: go to root of HD
/*
                    if(!viewNewDir(bdata,"/mnt"))
                        stopLs();*/
                    break;
                case BTN_F2:
#warning need to code CF specific actions 
/*                    if(CF_is_mounted())
                        if(!viewNewDir(bdata,"/cf"))
                            stopLs();*/
                    break;
                case BTN_F3:
#warning need to code action: show fct menu
/*
                    menu_cfg.root=&menu1;
                    evt_mode=MENU_MODE;
                    hideHelper();
                    start_menu(&menu_cfg);
                    menuEvtHandler(EVT_REDRAW);
                    */
                    break;
                case BTN_ON:
                    chgSelect(bdata,bdata->pos+bdata->nselect);
                    break;
                case BTN_OFF:
                case EVT_QUIT:
                    //hideHelper();
                    //cleanList(bdata);
                    //browser_listener.enable=0;
                    //restore_menu();
                    break;
                case EVT_REDRAW:
                    viewNewDir(bdata,NULL);
                    break;
#if 0                
                case EVT_CF_REMOVED:
#warning need to code CF specific actions                
                    /*getcwd(pwd, 10);
                    pwd[10]='\0';
                    if(pwd[0]=='/' && pwd[1]=='c' && pwd[2]=='f' && (pwd[3]=='/'||pwd[3]=='\0'))
                        if(!viewNewDir(bdata,"/mnt"))
                            stopLs();*/
                    break;
                case EVT_CF_ADDED:
#warning need to code CF specific actions 
                    /*if(!viewNewDir(bdata,"/cf"))
                        stopLs();*/
                    break;
#endif                    
            }
}

void ini_file_browser(void)
{
    bdata = &realData;
    iniBrowser();
}

void browse_root(void)
{
    start_browser("/");
}

int start_browser(char * path)
{      
    //iniHelperMenu(&browserMenu);

    if(!viewNewDir(bdata,path))
    {
        return 0;
    }
        
    return 1;

}

int x=320;

void draw_file_size(struct dir_entry * entry)
{
    int h,w;
    char tmpS[15];
    
    /* erase previsous drawing */

    fillRect(COLOR_WHITE,x, 230,320-x,10);
    if(entry->type == TYPE_FILE)
    {
        createSizeString(tmpS,entry->size);
        getStringS(tmpS,&w,&h);
        x=320-w;
        putS(COLOR_BLUE, COLOR_WHITE,x, 230, tmpS);
    }
}

void draw_bottom_status(struct browser_data *bdata)
{
    char tmp[100];
    char tmpS[15];
//    char pwd[PATHLEN];
//    int len=0;   
    
    createSizeString(tmpS,bdata->totSize);
        
    fillRect(COLOR_WHITE,2, 220,316,20);
#warning code to get pwd        
    /*if (!getcwd(pwd, PATHLEN))
    {
        printf( "Cannot get current directory\n");        
    }
    else
    {
        len=strlen(pwd);        
        putS(COLOR_BLUE, COLOR_WHITE,2, 220, pwd);  
    }*/

    snprintf(tmp,100,"%d %s, %d %s, %s",bdata->nbFile,bdata->nbFile>0?"files":"file",
            bdata->nbDir,bdata->nbDir>0?"folders":"folders",tmpS);
    printf("%s\n",tmp);
    
    putS(COLOR_BLUE, COLOR_WHITE,2, 230, tmp);    
}

void clear_status(struct browser_data *bdata)
{
    fillRect(COLOR_WHITE,2, 220,316,20);
}

void createSizeString(char * str,int Isize)
{
    //char * unit;
    //float size=Isize;
    if(str!=NULL)
    {
        /*if(size/1024>=1)
        {
            size/=1024;
            unit="Kb";
            if(size/1024>=1)
            {
                size/=1024;
                unit="Mb";
                if(size/1024>=1)
                {
                    size/=1024;
                    unit="Gb";
                }
            }
        }
        else 
            unit = "b";   */
#warning need FPU     
           
        sprintf(str,"%d b",(int)Isize);
    }
}
