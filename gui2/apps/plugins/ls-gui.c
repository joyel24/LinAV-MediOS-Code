/*
* ls-gui.c
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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include "graphics.h"
#include "events.h"
#include "colordef.h"
#include "font.h"
#include "cops.h"
#include "avevents.h"
#include "msgBox.h"

#define    false 0
#define    true  1
#define    SHOW_ALL   1
#define    LISTSIZE   256
#define    PATHLEN    256
#define    MAXPOS     21
#define    FILE_X_OFFSET 10
#define    ASCII_DEFAULT 65

#define    UP_ARROW     0
#define    DOWN_ARROW   1

#define    toLower(chr)  ((chr>64 && chr<91)?chr+32:chr)

/* icons*/
#include "ls-gui-icons.h"

int namesort(s1,s2)
char **s1;
char **s2;
{
    char * st1=*s1;
    char * st2=*s2;
    while(*st1!=0 && *st2!=0 && toLower(*st1) == toLower(*st2))
    {
        st1++;
        st2++;
    }

    return (toLower(*st1) - toLower(*st2));
}

void strlwr(char *s)
{
        while (*s)
        {       *s = tolower(*s);
                s++;
        }
}

char            **dir_list;
int             dir_listsize;
int             dir_listused;

char            **file_list;
int             file_listsize;
int             file_listused;

struct client_operations * cops;

#define TYPE_DIR    0
#define TYPE_FILE   1

struct dir_entry {
    char *name;
    int type;
};

struct dir_entry * list;
int                listused;
int pos,nselect;

int mode=1;

void do_off(void * data)
{
    cops->stop_menu();
    mode=1;
}
void do_on(void * data)
{
}
void do_right(void * data)
{
    char tmp[200];
    int reload = false;
    int buttonResult = 0;

    cops->stop_menu();
    mode=1;

    if(strcmp((char*)data, "Copy") == 0)
    {
        // Copy the current selection
//        sprintf(tmp,"cp %s CopyOf_%s",list[pos+nselect].name,list[pos+nselect].name);
//        system(tmp);
        cops->putS(COLOR_BLACK, COLOR_WHITE,5, 230, list[pos+nselect].name);

    }
    else if(strcmp((char*)data, "Delete") == 0)
    {
        buttonResult = cops->msgBox("Delete Warning", "Really delete it ?", MSGBOX_TYPE_OKCANCEL, MSGBOX_ICON_WARNING);

        if(buttonResult == MSGBOX_OK)
        {
            remove(list[pos+nselect].name);
            reload = true;
        }
    }
    else if(strcmp((char*)data, "Rename") == 0)
    {
        cops->putS(COLOR_BLACK, COLOR_WHITE,5, 230, list[pos+nselect].name);
    }
    else if(strcmp((char*)data, "New Dir") == 0)
    {
        mkdir("New Dir", S_IRWXU);
        reload = true;
    }

    if(reload == true)
    {
        doLs("./");
        printAllName(pos,nselect);

        if(listused>MAXPOS)
            showArrow(DOWN_ARROW);
    }
}
void do_F1(void * data)
{
}
void do_F2(void * data)
{
}
void do_F3(void * data)
{
}

void mk_item_str(void * data,char * str)
{
    sprintf(str,"%s",(char*)data);
}

struct menu_data menu_cfg = {
    useOwnDisp     : 1,
    x:200,y:120,width:8*16,height:100,
    dx:2,dy:2,
    txt_color      : COLOR_WHITE,
    bg_color       : COLOR_GREEN,
    select_color   : COLOR_BLUE,
    sub_color      : COLOR_RED,
    root           : NULL,
    right_action   : do_right,
    on_action      : do_on,
    off_action     : do_off,
    f1_action      : do_F1,
    f2_action      : do_F2,
    f3_action      : do_F3,
    item_str       : mk_item_str,
    submenu_str    : mk_item_str
};

struct menu_item rootMenu;
struct menu_item menu1;
struct menu_item menu2;
struct menu_item menu3;
struct menu_item menu4;

void DeleteChar(char* src, char* dest, int index)
{
   int cnt = 0;
    int dstindex = 0;

    for( cnt = 0; cnt < strlen(src); cnt++)
    {
       if(cnt != index)
        {
          dest[dstindex] = src[cnt];
            dstindex++;
        }
    }

    dest[dstindex] = '\0';
}

// Bereiche in folgender Reihenfolge:
// 33-44 Sonderzeichen
// 58-63 Sonderzeichen
// 91-96 Sonderzeichen
// 123-126 Sonderzeichen
// 45-57 Zahlen
// 32 Space
// 64-90 Grossbuchstaben
// 97-122 Kleinbuchstaben
// 192-255 Erweiterter Zeichensatz
int NextAscii(int lastAscii)
{
   int ascii = ASCII_DEFAULT;

   if(lastAscii == 32) // Space
      ascii = 64; // auf @ und Grossbuchstaben springen
    else if(lastAscii == 90)
      ascii = 97;
    else if(lastAscii == 122)
       ascii = 192;
    else if(lastAscii == 255)
      ascii = 33;
    else if(lastAscii == 44)
       ascii = 58;
    else if(lastAscii == 63)
      ascii = 91;
    else if(lastAscii == 96)
      ascii = 123;
    else if(lastAscii == 126)
      ascii = 45;
    else if(lastAscii == 57)
      ascii = 32;
    else
      ascii = lastAscii+1;

    return ascii;
}

int PrevAscii(int lastAscii)
{
   int ascii = ASCII_DEFAULT;

   if(lastAscii == 33)
       ascii = 255;
    else if(lastAscii == 192)
      ascii = 122;
    else if(lastAscii == 97)
       ascii = 90;
    else if(lastAscii == 64)
      ascii = 32;
    else if(lastAscii == 32)
      ascii = 57;
    else if(lastAscii == 45)
      ascii = 126;
    else if(lastAscii == 123)
      ascii = 96;
    else if(lastAscii == 91)
      ascii = 63;
    else if(lastAscii == 58)
      ascii = 44;
    else
      ascii = lastAscii-1;

    return ascii;
}

int is_script_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".sh") == 0;
}

int is_image_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".jpg") == 0
        || strcmp(extension, ".gif") == 0
        || strcmp(extension, ".bmp") == 0;
}

int is_mp3_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".mp3") == 0;
}

int is_text_type(char * extension)
{
   int retVal = 0;

    strlwr(extension);
   if(strcmp(extension, ".txt") == 0)
       retVal = 1;
   else if(strcmp(extension, ".cfg") == 0)
       retVal = 1;
   else if(strcmp(extension, ".c") == 0)
       retVal = 1;
   else if(strcmp(extension, ".cpp") == 0)
       retVal = 1;
   else if(strcmp(extension, ".h") == 0)
       retVal = 1;
   else if(strcmp(extension, ".ini") == 0)
       retVal = 1;
   else if(strcmp(extension, ".csv") == 0)
       retVal = 1;

    return retVal;
}

void showArrow(int type)
{
    int h=0,w=0;
    cops->getStringS("M", &w, &h);

    switch(type)
    {
        case UP_ARROW:
            cops->drawBITMAP(&upBitmap,310,h+MENU_SHADOW+1+6);
            break;
        case DOWN_ARROW:
//            cops->drawBITMAP(&dwBitmap,310,SCREEN_HEIGHT-10);
            cops->drawBITMAP(&dwBitmap,310,2+h+6+MENU_SHADOW+(MAXPOS-1)*(h+1));
            break;
    }
}

void hideArrow(int type)
{
    int h=0,w=0;
    cops->getStringS("M", &w, &h);

    switch(type)
    {
        case UP_ARROW:
            cops->fillRect(COLOR_WHITE,310,h+MENU_SHADOW+1+6,9,9);
            break;
        case DOWN_ARROW:
//            cops->fillRect(COLOR_WHITE,310,SCREEN_HEIGHT-10,9,9);
            cops->fillRect(COLOR_WHITE,310,2+h+6+MENU_SHADOW+(MAXPOS-1)*(h+1),9,9);
            break;
    }
}

int ini_lists(void)
{
    if (dir_listsize == 0) {
        dir_list = (char **) malloc(LISTSIZE * sizeof(char *));
        if (dir_list == NULL) {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        dir_listsize = LISTSIZE;
    }
    dir_listused = 0;

    if (file_listsize == 0) {
        file_list = (char **) malloc(LISTSIZE * sizeof(char *));
        if (file_list == NULL) {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        file_listsize = LISTSIZE;
    }
    file_listused = 0;
    return 0;
}

int add_dir(char * name)
{
    char            **newlist;

    if (dir_listused >= dir_listsize)
    {
        newlist = malloc((sizeof(char **)) * (dir_listsize + LISTSIZE));
        if (newlist == NULL)
        {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        memcpy(newlist, dir_list, sizeof(char**) * dir_listsize);
        free(dir_list);
        dir_list=newlist;
        dir_listsize += LISTSIZE;
    }

    dir_list[dir_listused] = strdup(name);
    if (dir_list[dir_listused] == NULL) {
        fprintf(stderr, "No memory for filenames\n");
        return -1;
    }
    dir_listused++;
    return 0;

}

int add_file(char * name)
{
    char            **newlist;

    if (file_listused >= file_listsize)
    {
        newlist = malloc((sizeof(char **)) * (file_listsize + LISTSIZE));
        if (newlist == NULL)
        {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        memcpy(newlist, file_list, sizeof(char**) * file_listsize);
        free(file_list);
        file_list=newlist;
        file_listsize += LISTSIZE;
    }

    file_list[file_listused] = strdup(name);
    if (file_list[file_listused] == NULL) {
        fprintf(stderr, "No memory for filenames\n");
        return -1;
    }
    file_listused++;
    return 0;

}

int doLs(char * name)
{
    DIR             *dirp;
    struct dirent   *dp;
    struct stat     statbuf;
    char            fullname[PATHLEN];
    int             endslash;
    int             i;

    endslash = (*name && (name[strlen(name) - 1] == '/'));

    if(ini_lists()<0)
        return -1;

    dirp = opendir(name);
    if (dirp == NULL) {
        perror(name);
        return -1;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        if ((dp->d_name[0] == '.') && !SHOW_ALL)
            continue;

        fullname[0] = '\0';

        if ((*name != '.') || (name[1] != '\0'))
        {
            strcpy(fullname, name);
            if (!endslash)
                strcat(fullname, "/");
        }

        strcat(fullname, dp->d_name);

        if (stat(dp->d_name, &statbuf) < 0)
        {
            perror(dp->d_name);
            return;
        }

        if(S_ISDIR(statbuf.st_mode))
        {
            if(add_dir(fullname)<0)
                return -1;
        }
        else
        {
            if(add_file(fullname)<0)
                return -1;
        }
    }

    closedir(dirp);

    qsort((char *) dir_list, dir_listused, sizeof(char *), namesort);
    qsort((char *) file_list, file_listused, sizeof(char *), namesort);

    listused=dir_listused+file_listused;

    list=(struct dir_entry*)malloc((sizeof(struct dir_entry))*listused);

    if (list == NULL)
    {
        fprintf(stderr, "No memory for ls buffer\n");
        return -1;
    }

    for(i=0;i<dir_listused;i++)
    {
        list[i].name=dir_list[i];
        list[i].type=TYPE_DIR;
    }

    for(i=0;i<file_listused;i++)
    {
        list[dir_listused+i].name=file_list[i];
        list[dir_listused+i].type=TYPE_FILE;
    }

    free(dir_list);
    free(file_list);

    return 0;
}

int printName(struct dir_entry * dEntry,int x,int y,int clear,int selected)
{
    //struct stat     statbuf;
    int             color;
    char *          cp;
    int             w = 0;
    int             h = 10;
    char *ext;

   cops->getStringS("M", &w, &h);

    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        cops->fillRect(COLOR_WHITE, 0, y , 320, h+1);

    if(dEntry->type == TYPE_DIR)
    {
        color=COLOR_RED;
        cops->drawBITMAP (&dirBitmap, 2, y);
    }
    else
    {
        color=COLOR_BLACK;

        ext = strrchr(dEntry->name, '.');
        if (ext == 0)
        {
            // no extension
            cops->fillRect(COLOR_WHITE, 2, y, 8, 8);
        }
        else if (is_mp3_type(ext))
            cops->drawBITMAP (&mp3Bitmap, 2, y);
        else if(is_text_type(ext))
            cops->drawBITMAP (&textBitmap, 2, y);
        else if(is_image_type(ext))
            cops->drawBITMAP (&imageBitmap, 2, y);
        else
            cops->fillRect(COLOR_WHITE, 2, y, 8, 8);
    }

    if(selected)
        cops->putS(color, COLOR_BLUE,x, y, dEntry->name);
    else
        cops->putS(color, COLOR_WHITE,x, y, dEntry->name);
}

void printAllName(int pos,int nselect)
{
    int w = 0;
    int h = 10;
    int i;

    cops->getStringS("M", &w, &h);

    for (i = pos; i < listused && i < pos+MAXPOS; i++)
    {
        cops->fillRect(COLOR_WHITE,0, 2+(i-pos)*(h+1)+ h+6+MENU_SHADOW , 320,(h+1));
        printName(&list[i],FILE_X_OFFSET, 2 + (i-pos)*(h+1)+ h+6+MENU_SHADOW,0,(i-pos)==nselect);
    }

    for(;i<pos+MAXPOS;i++)
        cops->fillRect(COLOR_WHITE,0, (i-pos)*(h+1)+ h+6+MENU_SHADOW , 320,(h+1));
}

void printAName(int pos, int nselect, int clear, int selected)
{
    int w = 0;
    int h = 10;

    cops->getStringS("M", &w, &h);

    printName(&list[pos],FILE_X_OFFSET,2 + nselect*(h+1)+ h+6+MENU_SHADOW,clear,selected);
}

void cleanList()
{
    int i;
    for (i = 0; i < listused; i++)
    {
        free(list[i].name);
        free(&list[i]);
    }

    listused=0;
}


#define MAXargs 31

//execBin(path,arg0,arg1,arg2,...,(char*)0) arg0 should be the name of the app
int execBin(char * path, ...)
{
    int pid;
    va_list ap;
    char *array [MAXargs];
    int argno=0;

    cops->clearEventQueue();
    pid = vfork();
    if (pid == 0)
    {
        va_start (ap, path);
        while ((array[argno++] = va_arg(ap, char*)) != (char*)0) /* NOTHING */ ;
        va_end(ap);
        execv(path, array);

        fprintf(stderr, "exec failed!\n");
        _exit(1);
    }
    else
    {
        if (pid > 0)
        {
            int status;
            cops->closeScreen();
            waitpid(pid, &status, 0);
            cops->openScreen();
        }
        else
        {
            fprintf(stderr, "vfork failed %d\n", pid);
        }
    }
}

int launchBin(char * name)
{
    execBin(name, name, (char*)0);
}

void launchSoundPlayer(char *name)
{
    char vol[5];
    char rep[5];
    sprintf(vol, "%d",70);
    sprintf(rep, "%d",0);

    execBin("/mnt/avwm/apps/play", "play", name, vol, rep, (char*)0);
}

void launchViewer(char *name)
{
    execBin("/mnt/avwm/apps/viewer", "viewer", name, (char*)0);
}

void launchTxtView(char *name)
{
    execBin("/mnt/avwm/apps/txtviewer", "txtviewer", name, (char*)0);
}

int launchScript(char * name)
{
    execBin("/bin/sh", "sh", name, (char*)0);
}

void handle_type_other(char *filename)
{
    char *ext;

    ext = strrchr(filename, '.');
    if (ext == 0)
    {
      // no extension
        launchBin(filename);
    }
    else if (is_image_type(ext))
    {
       launchViewer(filename);
    }
    else if (is_text_type(ext))
    {
       launchTxtView(filename);
    }
    else if (is_mp3_type(ext))
    {
       //launchSoundPlayer(filename);
       cops->playMp3(filename);
    }
    else if (is_script_type(ext))
    {
       launchScript(filename);
    }
    else
    {
       // unknown type
    }
}

int eventHandler(int evt)
{
    /*char tmp[50];
    sprintf(tmp,"in handler (%d)",evt);
    cops->putS(BLACK, WHITE,0, 220, tmp);*/
    int w = 0;
    int h = 10;
//    char str[40];

    cops->getStringS("M", &w, &h);

    if(mode)
    {
        switch(evt)
        {
            case BTN_UP:
                nselect--;

                if(nselect<0)
                {
                    nselect=0;
                    pos--;
                    if(pos<0) // we are at the beg => do wrapping
                    {
                        pos=listused-MAXPOS;//-1;
                        if(pos<0)
                        {
                            pos=0;
                            nselect=listused-1;
                        }
                        else
                            nselect=listused-pos-1;
                        printAllName(pos,nselect);
                        //pos=0;
                    }
                    else // not going up, scrolling
                    {
                        cops->scrollWindowVert(COLOR_WHITE, 0, 1  + h+6+MENU_SHADOW, 320, (h+1)*MAXPOS, h+1,0);
                        printAName(pos+nselect+1,nselect+1,1,0);
                        printAName(pos+nselect,nselect,1,1);
                    }
                }
                else // just going up
                {
                    printAName(pos+nselect+1,nselect+1,0,0);
                    printAName(pos+nselect,nselect,0,1);
                }

    /*
                sprintf(str,"pos: %2d select %2d used: %2d", pos, nselect, listused);
                cops->fillRect(COLOR_BLUE, 0, 229, 250,12);
                cops->putS(COLOR_WHITE, COLOR_BLUE,5, 230, str);
    */
                if( (listused>MAXPOS) &&
                    (pos+MAXPOS < listused) )
                    showArrow(DOWN_ARROW);
                else
                    hideArrow(DOWN_ARROW);

                if(pos == 0)
                    hideArrow(UP_ARROW);
                else
                    showArrow(UP_ARROW);

                break;

            case BTN_DOWN:
                nselect++;
                if(nselect+pos>=listused)
                {
                    // jump to beginning
                    pos=0;
                    nselect=0;
                    printAllName(pos,nselect);
                }
                else
                {
                    if(nselect>=MAXPOS)
                    {
                        nselect=MAXPOS-1;
                        pos++;
                        if(pos+MAXPOS>listused) // we are at the end => do wrapping
                        {
                            //pos=listused-MAXPOS-1;
                            pos=0;
                            nselect=0;
                            printAllName(pos,nselect);
                        }
                        else // not going down, scrolling
                        {
                            cops->scrollWindowVert(COLOR_WHITE, 0, 1  + h+6+MENU_SHADOW, 320, (h+1)*MAXPOS, h+1,1);
                            printAName(pos+nselect-1,nselect-1,1,0);
                            printAName(pos+nselect,nselect,1,1);
                        }
                    }
                    else
                    {
                        printAName(pos+nselect-1,nselect-1,0,0);
                        printAName(pos+nselect,nselect,0,1);
                    }
                }
    /*
                sprintf(str,"pos: %2d select %2d used: %2d", pos, nselect, listused);
                cops->fillRect(COLOR_BLUE, 0, 229, 250,12);
                cops->putS(COLOR_WHITE, COLOR_BLUE,5, 230, str);
    */
                if(pos>0)
                    showArrow(UP_ARROW);
                else
                    hideArrow(UP_ARROW);

                if( (listused>MAXPOS) &&
                    (pos+MAXPOS < listused) )
                    showArrow(DOWN_ARROW);
                else
                    hideArrow(DOWN_ARROW);

                break;

            case BTN_RIGHT:
                if(chdir(list[pos+nselect].name)<0)
                    handle_type_other(list[pos+nselect].name);
                else
                {
                    cleanList();
                    if(doLs("./")<0)
                    {
                        listused = 0;
                        return -1;
                    }
                    hideArrow(DOWN_ARROW);
                    hideArrow(UP_ARROW);
                    if(listused>MAXPOS)
                        showArrow(DOWN_ARROW);
                    pos=0;
                    nselect=0;
                    //cops->fillRect(WHITE,5, h+6+MENU_SHADOW , 305,(h+1)*MAXPOS);
                    printAllName(pos,nselect);
                    //cops->clearEventQueue();
                }
                break;
            case BTN_LEFT:
                if(chdir("../")<0)
                    break;
                cleanList();
                if(doLs("./")<0)
                {
                    listused = 0;
                    return -1;
                }
                pos=0;
                nselect=0;
                //cops->fillRect(WHITE,5, h+6+MENU_SHADOW , 305,(h+1)*MAXPOS);
                hideArrow(DOWN_ARROW);
                hideArrow(UP_ARROW);
                if(listused>MAXPOS)
                    showArrow(DOWN_ARROW);
                printAllName(pos,nselect);
                //cops->clearEventQueue();
                break;

            case BTN_F3:
                menu_cfg.root=&menu1;
                mode=0;
                cops->start_menu(&menu_cfg);
                cops->menuEvtHandler(EVT_REDRAW);
                break;

            case BTN_OFF:
            case EVT_QUIT:
                RELEASE(cops)
                break;
            case EVT_REDRAW:
                //cops->fillRect(WHITE,5, h+6+MENU_SHADOW , 305,225);
                printAllName(pos,nselect);
                break;
        }
    }
    else
    {
        cops->menuEvtHandler(evt);
    }
}

int main(int argc,char * * argv)
{
    int evt;
    int w = 0;
    int h = 10;

    REGISTER(cops,eventHandler,0);

    pos=0;
    nselect=0;

    if(argc>0)
    {
        cops->disableMenu();
        cops->setFont(STD6X9);

        menu1.data=(void*)"Rename";
        menu2.data=(void*)"Delete";
        menu3.data=(void*)"Copy";
        menu4.data=(void*)"New Dir";

        menu1.prev=NULL;
        menu1.nxt=&menu2;
        menu1.sub=NULL;
        menu1.up=NULL;

        menu2.prev=&menu1;
        menu2.nxt=&menu3;
        menu2.sub=NULL;
        menu2.up=NULL;

        menu3.prev=&menu2;
        menu3.nxt=&menu4;
        menu3.sub=NULL;
        menu3.up=NULL;

        menu4.prev=&menu3;
        menu4.nxt=NULL;
        menu4.sub=NULL;
        menu4.up=NULL;

        listused = 0;

        if(strlen(argv[1]) == 1)
        {
            if(argv[0] = '/')
                chdir("./"); // handle spezial case if path only / without dot
            else
                chdir(argv[1]);
        }
        else
            chdir(argv[1]);

        if(doLs("./")<0)
        {
            listused = 0;
            return -1;
        }

//        cops->getStringS("M", &w, &h);
        //cops->fillRect(WHITE,5, h+6+MENU_SHADOW , 315,240-h-6-MENU_SHADOW);
        printAllName(pos,nselect);

        if(listused>MAXPOS)
            showArrow(DOWN_ARROW);

        PACK(cops,NULL);

        cleanList();
//        STOPME(cops);
        return 0;
    }
    STOPME(cops)
    return -1;
}
