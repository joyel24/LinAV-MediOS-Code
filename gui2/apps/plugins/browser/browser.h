/*
* ls_main.h
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

#ifndef __BROWSER_H
#define __BROWSER_H

#define TYPE_DIR    1
#define TYPE_FILE   2
#define TYPE_BACK   0

#define LEFT_SCROLL  0
#define RIGHT_SCROLL 1

#define PATHLEN    256
#define MAX_CHAR   100;

#define BRW_MODE    0
#define MENU_MODE   1
#define CP_MV_MODE  2

#define MODE_COPY   0
#define MODE_MOVE   1

struct dir_entry {
    char * name;
    int type;
    int size;
    int selected;
};

struct browser_data {
    char * path;
    
    int pos;
    int nselect;
    
    int show_dot_files;
    
    int scroll_pos;

    int nbFile;
    int nbDir;
    int totSize;
    
    int nb_disp_entry;
    int x_start;
    int y_start;
    
    int width;
    int height;
    
    int entry_height;
    
    struct dir_entry * list;
    int                listused;
    int                listsize;
    
    void (*draw_bottom_status)  (struct browser_data *bdata);
    void (*draw_file_size)      (struct dir_entry * entry);
    void (*clear_status)        (struct browser_data *bdata);
    
    
};

//#define MENU_SHADOW 2 // height of small shadow under the title

//#define    UP_ARROW     0
//#define    DOWN_ARROW   1



/*****    ls_fct    *****/
int  qSortEntry  (const void * a1,const void * a2);
void cleanList   (struct browser_data * bdata);
int  ini_lists   (struct browser_data * bdata);
int  addEntry    (struct browser_data * bdata,char * name,int type,int size);
int  doLs        (struct browser_data * bdata,char * name);
void chgSelect   (struct browser_data *bdata,int num);
int  nbSelected  (struct browser_data * bdata);
struct dir_entry * nxtSelect(struct browser_data * bdata,int * pos);

/*****    evt_handle_fct    *****/
int  browserEvt         (int evt,struct browser_data * bdata);

/*****    gui_fct    *****/
void iniBrowser         (void);
int  viewNewDir         (struct browser_data *bdata,char *name);
void  printName          (struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata);
void printAllName       (struct browser_data *bdata);
void printAName         (struct browser_data *bdata,int pos, int nselect, int clear, int selected);
void draw_bottom_status (struct browser_data *bdata);
void draw_file_size     (struct dir_entry * entry);
void createSizeString   (char * str,int Isize);
void clearBrowser       (struct browser_data *bdata);

#endif
