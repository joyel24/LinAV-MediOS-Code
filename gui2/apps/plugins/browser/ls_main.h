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

#include "cops.h"
#include "avevents.h"
#include "graphics.h"
#include "events.h"
#include "colordef.h"
#include "font.h"


#define TYPE_DIR    0
#define TYPE_FILE   1
struct dir_entry {
    char * name;
    int type;
    int size;
};

#define    UP_ARROW     0
#define    DOWN_ARROW   1

#define    MAXPOS     20

#define    false 0
#define    true  1
#define    FALSE 0
#define    TRUE  1

#define PATHLEN    256
#define MAX_CHAR   100;

/*****    str_fct   *****/
int  namesort          (char **s1,char **s2);
void strlwr            (char *s);
void createSizeString  (char * str,int Isize);

/*****    ls_fct    *****/
int  qSortEntry  (const void * a1,const void * a2);
void cleanList   (void);
int  ini_lists   (void);
int  add_dir     (char * name);
int  add_file    (char * name);
int  doLs        (char * name);

/*****    file_handle_fct    *****/
int   is_script_type     (char * extension);
int   is_image_type      (char * extension);
int   is_mp3_type        (char * extension);
int   is_text_type       (char * extension);
int   execBin            (char * path, ...);
int   launchBin          (char * name);
void  launchSoundPlayer  (char * name);
void  launchViewer       (char * name);
void  launchTxtView      (char * name);
int   launchScript       (char * name);
void  handle_type_other  (char * filename);
int   do_mv              (char * src,char * dest);
int   do_cp              (char * src,char * dest);
int   isadir             (char * name);
char* buildname          (char * dirname,char * filename);
int   copyfile           (char * src,char * dest,int setmodes);

/*****    evt_handle_fct    *****/
int  eventHandler       (int evt);

/*****    menu_fct    *****/
void ini_menu_struct  (void);
void do_off           (void * data);
void do_on            (void * data);
void do_right         (void * data);
void do_F1            (void * data);
void do_F2            (void * data);
void do_F3            (void * data);
void mk_item_str      (void * data,char * str);

/*****    gui_fct    *****/
int  viewNewDir         (char *name);
void showArrow          (int type);
void hideArrow          (int type);
int  printName          (struct dir_entry * dEntry,int x,int y,int clear,int selected);
void printAllName       (int pos,int nselect);
void printAName         (int pos, int nselect, int clear, int selected);
void draw_bottom_status (void);
void draw_file_size     (struct dir_entry * entry);
