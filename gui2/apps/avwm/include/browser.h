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

struct dir_entry {
    char * name;
    int type;
    int size;
};

struct browser_data {
    char * path;
    
    struct dir_entry * list;
    int                listused;
    int                listsize;
    
    int pos;
    int nselect;
    
    int show_dot_files;

    int nbFile;
    int nbDir;
    int totSize;
    
    int nb_disp_entry;
    int x_start;
    int y_start;
    
    int width;
};

#define MENU_SHADOW 2 // height of small shadow under the title

#define    UP_ARROW     0
#define    DOWN_ARROW   1

//#define    MAXPOS     20


#define PATHLEN    256
#define MAX_CHAR   100;


/*****    ls_fct    *****/
int  qSortEntry  (const void * a1,const void * a2);
void cleanList   (struct browser_data * bdata);
int  ini_lists   (struct browser_data * bdata);
int  addEntry    (struct browser_data * bdata,char * name,int type,int size);
int  doLs        (struct browser_data * bdata,char * name);

/*****    evt_handle_fct    *****/
int  browserEvt         (int evt,struct browser_data * bdata);


/*****    gui_fct    *****/
int  viewNewDir         (struct browser_data *bdata,char *name);
void showArrow          (int type,int max);
void hideArrow          (int type,int max);
int  printName          (struct dir_entry * dEntry,int x,int y,int clear,int selected);
void printAllName       (struct browser_data *bdata);
void printAName         (struct browser_data *bdata,int pos, int nselect, int clear, int selected);
void draw_bottom_status (struct browser_data *bdata);
void draw_file_size     (struct dir_entry * entry);

#endif