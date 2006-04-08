/*
* include/gui/file_browser.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_FILE_BROWSER_H
#define __GUI_FILE_BROWSER_H

#include <gui/gui.h>

struct dir_entry {
    char * name;
    int type;
    int size;
    int selected;
};

struct browser_data {

    char path[PATHLEN];
    
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
    
    int mode;
    int font;
    
    struct dir_entry * list;
    int                listused;
    int                listsize;
    
    void (*draw_bottom_status)  (struct browser_data *bdata);
    void (*draw_file_size)      (struct dir_entry * entry);
    void (*clear_status)        (struct browser_data *bdata);
    
    char * retPath;
    
};

//#define MENU_SHADOW 2 // height of small shadow under the title

//#define    UP_ARROW     0
//#define    DOWN_ARROW   1

int browser_simpleBrowse(char * path,char * res);
int browser_browse(struct browser_data *bdata,char * path,char * res);

void browser_disposeBrowse(struct browser_data * bdata);
struct browser_data * browser_NewBrowse(void);

/*****    ls_fct    *****/
int  qSortEntry  (const void * a1,const void * a2);
void cleanList   (struct browser_data * bdata);
int  ini_lists   (struct browser_data * bdata);
int  addEntry    (struct browser_data * bdata,char * name,int type,int size);
int  doLs        (struct browser_data * bdata);
void chgSelect   (struct browser_data *bdata,int num);
int  nbSelected  (struct browser_data * bdata);
struct dir_entry * nxtSelect(struct browser_data * bdata,int * pos);

int upDir(struct browser_data * bdata);
int inDir(struct browser_data * bdata,char * name);
int isRoot(struct browser_data * bdata);

/*****    evt_handle_fct    *****/
int  browserEvt         (struct browser_data * bdata);

/*****    gui_fct    *****/
void iniBrowser         (void);
int  viewNewDir         (struct browser_data *bdata,char *name);
void printName          (struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata);
void printAllName       (struct browser_data *bdata);
void printAName         (struct browser_data *bdata,int pos, int nselect, int clear, int selected);
void draw_bottom_status (struct browser_data *bdata);
void draw_file_size     (struct dir_entry * entry);
void createSizeString   (char * str,int Isize);
void clearBrowser       (struct browser_data *bdata);
void redrawBrowser      (struct browser_data *bdata);

/* main fct */
void  bwseventHandler       (int evt);

/**** drawings        ******/
void draw_bottom_status  (struct browser_data *bdata);
void draw_file_size      (struct dir_entry * entry);
void clear_status(struct browser_data *bdata);

//char *  browse     (char * path,int mode);
//void ini_file_browser  (void);




#endif
