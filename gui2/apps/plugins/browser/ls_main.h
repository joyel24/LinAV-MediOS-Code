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
#include "file_type.h"

#define    false 0
#define    true  1
#define    FALSE 0
#define    TRUE  1

#define PATHLEN    256
#define MAX_CHAR   100;

#define BRW_MODE    0
#define MENU_MODE   1
#define CP_MV_MODE  2

int  eventHandler       (int evt);

/*****    menu_fct    *****/
void ini_menu_struct   (struct browser_data *bdata);
void do_off            (void * data);
void do_on             (void * data);
void do_right          (void * data);
void do_F1             (void * data);
void do_F2             (void * data);
void do_F3             (void * data);
void mk_item_str       (void * data,char * str);
void cp_mv_evt         (int evt);
void draw_cp_mv_bottom (struct browser_data *bdata);

/**** drawings        ******/
void draw_bottom_status  (struct browser_data *bdata);
void draw_file_size      (struct dir_entry * entry);
