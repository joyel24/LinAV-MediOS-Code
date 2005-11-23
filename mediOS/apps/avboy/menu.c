/*********************************************************************/
/* menu.c - user menu for rockboy                                    */
/*                                                                   */
/* Note: this file only exposes one function: do_user_menu().        */
/*********************************************************************/

//#include <stdlib.h>"
//#include "string.h"
//#include "button.h"
//#include "rockmacros.h"
//#include <sys_def/string.h>
#include <sys_def/types.h>
#include <sys_def/ctype.h>
#include "mem.h"
#include <fs_io.h>
//#include <buttons.h>
//#include "graph.h"
//#include "types.h"
#include <api.h>
#include <evt.h>


#define getstringsize(a,b,c) getStringS(a,b,c)
#define drawline(a,b,c,d,e) drawLine(e,a,b,c,d)
#define drawrect(a,b,c,d,e) drawRect(e,a,b,c,d)
#define fillrect(a,b,c,d,e) fillRect(e,a,b,c,d)
#define putsxy(a,b,c) putS(0xFF,0x00,a,b,c)

 
#define OSD_BITMAP1_WIDTH 160
#define OSD_BITMAP1_HEIGHT 144

#define USER_MENU_QUIT -2

#define NULL 0

/* load/save state function declarations */
static void do_slot_menu(bool is_load);
static void do_opt_menu(void);
static void munge_name(char *buf, size_t bufsiz);

/* directory ROM save slots belong in */
#define STATE_DIR "/avboy"

#define MENU_CANCEL (-1)
static int do_menu(char *title, char **items, size_t num_items, int sel_item);

/* main menu items */
#define MAIN_MENU_TITLE "AVBoy"
typedef enum {
  MM_ITEM_BACK,
  MM_ITEM_LOAD,
  MM_ITEM_SAVE,
  MM_ITEM_OPT,
  MM_ITEM_QUIT,
  MM_ITEM_LAST
} MainMenuItem;

/* strings for the main menu */
static const char *main_menu[] = {
  "Back to Game",
  "Load State...",
  "Save State...",
  "Options...",
  "Quit AVBoy"
};

typedef enum {
  SM_ITEM_SLOT1,
  SM_ITEM_SLOT2,
  SM_ITEM_SLOT3,
  SM_ITEM_SLOT4,
  SM_ITEM_SLOT5,
  SM_ITEM_FILE,
  SM_ITEM_BACK,
  SM_ITEM_LAST
} SlotMenuItem;

/* this semi-evil, but we snprintf() into these strings later
 * Note: if you want more save slots, just add more lines 
 * to this array */
static const char *slot_menu[] = {
  "1.                  ",
  "2.                  ",
  "3.                  ",
  "4.                  ",
  "5.                  ",
  "Save to File...     ",
  "Previous Menu...    "
};

#define OPT_MENU_TITLE "Options"
typedef enum {
  OM_ITEM_BACK,
  OM_MENU_LAST
} OptMenuItem;

static const char *opt_menu[] = {
  "Previous Menu..."
};

/*
 * do_user_menu - create the user menu on the screen.
 *
 * Returns USER_MENU_QUIT if the user selected "quit", otherwise 
 * returns zero.
 *
 * Note: this is the only non-static function in this file at the
 * moment.  In the future I may turn do_menu/etc into a proper API, in
 * which case they'll be exposed as well.
 *
 */
int do_user_menu(void) {
  int mi, ret, num_items;
  bool done = false;

  /* set defaults */
  ret = 0; /* return value */
  mi = 0; /* initial menu selection */
  num_items = sizeof(main_menu) / sizeof(char*);
  
  /* loop until we should exit menu */
  while (!done) {
    /* get item selection */
    mi = do_menu(MAIN_MENU_TITLE, (char**) main_menu, num_items, mi);
    
    /* handle selected menu item */
    switch (mi) {
      case MM_ITEM_QUIT:
        ret = USER_MENU_QUIT;
      case MENU_CANCEL:
      case MM_ITEM_BACK:
        done = true;
        break;
      case MM_ITEM_LOAD:
        do_slot_menu(true);
        break;
      case MM_ITEM_SAVE:
        do_slot_menu(false);
        break;
      case MM_ITEM_OPT:
        do_opt_menu();
        break;
    }
  }

  /* return somethin' */
  return ret;
}

/*
 * munge_name - munge a string into a filesystem-safe name
 */
static void munge_name(char *buf, const size_t bufsiz) {
  unsigned int i, max;

  /* check strlen */
  max = strlen(buf);
  max = (max < bufsiz) ? max : bufsiz;
  
  /* iterate over characters and munge them (if necessary) */
  for (i = 0; i < max; i++)
    if (!isalnum(buf[i]))
      buf[i] = '_';
}

/*
 * build_slot_path - build a path to an slot state file for this rom
 *
 * Note: uses rom.name.  Is there a safer way of doing this?  Like a ROM
 * checksum or something like that?
 */
static void build_slot_path(char *buf, size_t bufsiz, size_t slot_id) {
  char name_buf[40];

  /* munge state file name */
  strncpy(name_buf, rom.name, sizeof(name_buf));
  name_buf[16] = '\0';
  munge_name(name_buf, strlen(name_buf));

  /* glom the whole mess together */
  snprintf(buf, bufsiz, "%s/%s-%d.rbs", STATE_DIR, name_buf, slot_id + 1);
}

/*
 * do_file - load or save game data in the given file
 *
 * Returns true on success and false on failure.
 *
 * @desc is a brief user-provided description (<20 bytes) of the state.
 * If no description is provided, set @desc to NULL.
 *
 */
static bool do_file(char *path, char *desc, bool is_load) {
  char buf[200], desc_buf[20];
  int fd, file_mode;
    
  /* set file mode */
  //file_mode = is_load ? O_RDONLY : ();
  
  /* attempt to open file descriptor here */
  if ((fd = fopen(path, is_load ? O_RDONLY : O_WRONLY | O_CREAT)) <= 0)
    return false;

  /* load/save state */
  if (is_load) {
    /* load description */
    fread(fd, desc_buf, 20);

    /* load state */
    loadstate(fd);

    /* print out a status message so the user knows the state loaded */
    snprintf(buf, sizeof(buf), "Loaded state from \"%s\"", path);
  //splash(buf);
  } else {
    /* build description buffer */
    memset(desc_buf, 0, sizeof(desc_buf));
    if (desc)
      strncpy(desc_buf, desc, sizeof(desc_buf));

    /* save state */
   /* fwrite(fd, desc_buf, 20);
    savestate(fd);*/
  }

  /* close file descriptor */
  fclose(fd);

  /* return true (for success) */
  return true;
}

/*
 * do_slot - load or save game data in the given slot
 *
 * Returns true on success and false on failure.
 */
static bool do_slot(size_t slot_id, bool is_load) {
  char path_buf[256], desc_buf[20];
 // char path_buf[256], desc_buf="teste";
  
  /* build slot filename, clear desc buf */
  build_slot_path(path_buf, sizeof(path_buf), slot_id);
  memset(desc_buf, 0, sizeof(desc_buf));

  /* if we're saving to a slot, then get a brief description */
  if (!is_load) {
  //  if (rb->kbd_input(desc_buf, sizeof(desc_buf)) || !strlen(desc_buf)) {
    if (!strlen(desc_buf)) {
      memset(desc_buf, 0, sizeof(desc_buf));
      strncpy(desc_buf, "Untitled", sizeof(desc_buf));
    }
  }

  /* load/save file */
  return do_file(path_buf, desc_buf, is_load);
}

/* 
 * get information on the given slot
 */
static void slot_info(char *info_buf, size_t info_bufsiz, size_t slot_id) {
  char buf[256];
  int fd;

  /* get slot file path */
  build_slot_path(buf, sizeof(buf), slot_id);

  /* attempt to open slot */
  if ((fd = fopen(buf, O_RDONLY)) >= 0) {
    /* this slot has a some data in it, read it */
    if (fread(fd, buf, 20) > 0) {
      buf[20] = '\0';
      snprintf(info_buf, info_bufsiz, "%2d. %s", slot_id + 1, buf);
    } else {
      snprintf(info_buf, info_bufsiz, "%2d. ERROR", slot_id + 1);
    }
    fclose(fd);
  } else {
    /* if we couldn't open the file, then the slot is empty */
    snprintf(info_buf, info_bufsiz, "%2d.", slot_id + 1);
  }
}

/*
 * do_slot_menu - prompt the user for a load/save memory slot
 */
static void do_slot_menu(bool is_load) {
  int i, mi, ret, num_items;
  bool done = false;
  char *title, buf[256];

  /* set defaults */
  ret = 0; /* return value */
  mi = 0; /* initial menu selection */
  num_items = sizeof(slot_menu) / sizeof(char*);
  
  /* create menu items (the last two are file and previous menu,
   * so don't populate those) */
  for (i = 0; i < num_items - 2; i++)
    slot_info((char*) slot_menu[i], 20, i);
  
  /* set text of file item */
  snprintf((char*) slot_menu[SM_ITEM_FILE], 20, "%s File...", is_load ? "Load from" : "Save to");
  
  /* set menu title */
  title = is_load ? "Load State" : "Save State";

  /* loop until we should exit menu */
  while (!done) {
    /* get item selection */
    mi = do_menu(title, (char**) slot_menu, num_items, mi);
    
    /* handle selected menu item */
    done = true;
    if (mi != MENU_CANCEL && mi != SM_ITEM_BACK) {
      if (mi == SM_ITEM_FILE) {
        char rom_name_buf[40];

        /* munge rom name to valid filename */
        strncpy(rom_name_buf, rom.name, 16);
        munge_name(rom_name_buf, sizeof(rom_name_buf));

        /* create default filename */
        snprintf(buf, sizeof(buf), "/%s.rbs", rom_name_buf);

        /* prompt for output filename, save to file */
  //      if (!rb->kbd_input(buf, sizeof(buf)))
        if (0)
          done = do_file(buf, NULL, is_load);
      } else {
        done = do_slot(mi, is_load);
      }

      /* if we couldn't save the state file, then print out an
       * error message */
     // if (!is_load && !done)
     // splash("Couldn't save state file.");
    }
  }
}

static void do_opt_menu(void) {
  int mi, num_items;
  bool done = false;

  /* set a couple of defaults */
  num_items = sizeof(opt_menu) / sizeof(char*);
  mi = 0;
  
  while (!done) {
    mi = do_menu(OPT_MENU_TITLE, (char**) opt_menu, num_items, mi);
    if (mi == MENU_CANCEL || mi == OM_ITEM_BACK)
      done = true;
  }
}

/*********************************************************************/
/*  MENU FUNCTIONS                                                   */
/*********************************************************************/
/* at some point i'll make this a generic menu interface, but for now,
 * these defines will suffice */
#define MENU_X 10
#define MENU_Y 8
#define MENU_WIDTH (OSD_BITMAP1_WIDTH - 2 * MENU_X)
#define MENU_HEIGHT (OSD_BITMAP1_HEIGHT - 2 * MENU_Y)
#define MENU_RECT MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT
#define SHADOW_RECT MENU_X + 1, MENU_Y + 1, MENU_WIDTH, MENU_HEIGHT
#define MENU_ITEM_PAD 2

/*
 * select_item - select menu item (after deselecting current item)
 */
static void select_item(char *title, int curr_item, size_t item_i) {
  int x, y, w, h;

  /* get size of title, use that as height ofr all lines */
  getstringsize(title, &w, &h);
  h += MENU_ITEM_PAD * 2;

  /* calc x and width */
  x = MENU_X + MENU_ITEM_PAD;
  w = MENU_WIDTH - 2 * MENU_ITEM_PAD;

 // rb->lcd_set_drawmode(DRMODE_COMPLEMENT);
  /* if there is a current item, then deselect it */
  if (curr_item >= 0) {
    /* deselect old item */
    y = MENU_Y + h + MENU_ITEM_PAD * 2; /* account for title */
    y += h * curr_item;
 //   rb->lcd_fillrect(x, y, w, h);
    drawrect(x, y, w, h,0x00); //faute de complement!!
  }

  /* select new item */
  curr_item = item_i;

  /* select new item */
  y = MENU_Y + h + MENU_ITEM_PAD * 2; /* account for title */
  y += h * curr_item;
  drawrect(x, y, w, h,0xf9);
 // rb->lcd_set_drawmode(DRMODE_SOLID);

  /* update the menu window */
//  rb->lcd_update_rect(MENU_RECT);
}

/*
 * draw_menu - draw menu on screen
 *
 * Returns MENU_CANCEL if the user cancelled, or the item number of the
 * selected item.
 *
 */
static void draw_menu(char *title, char **items, size_t num_items)  {
  size_t i;
  int x, y, w, h, by;

  /* set to default? font */
 // rb->lcd_setfont(0);
  
  /* draw the outline */
  fillRect(0xaf,SHADOW_RECT);
 // rb->lcd_set_drawmode(DRMODE_SOLID|DRMODE_INVERSEVID);
  fillRect(0x00,MENU_RECT);
//  rb->lcd_set_drawmode(DRMODE_SOLID);
  drawRect(0xff,MENU_RECT);

  /* calculate x/y */
  x = MENU_X + MENU_ITEM_PAD;
  y = MENU_Y + MENU_ITEM_PAD * 2;
  getstringsize(title, &w, &h);
  h += MENU_ITEM_PAD * 2;

  /* draw menu stipple */
  for (i = MENU_Y; i < (size_t) y + h; i += 2)
    drawline(MENU_X, i, MENU_X + MENU_WIDTH-1, i,0xff);

  /* clear title rect */
//  rb->lcd_set_drawmode(DRMODE_SOLID|DRMODE_INVERSEVID);
  fillrect((OSD_BITMAP1_WIDTH - w) / 2 - 2, y - 2, w + 4, h,0x00);
//  rb->lcd_set_drawmode(DRMODE_SOLID);

  /* draw centered title on screen */
  putsxy((OSD_BITMAP1_WIDTH - w)/2, y, title);
  
  /* calculate base Y for items */
  by = y + h + MENU_ITEM_PAD;
  
  /* iterate over each item and draw it on the screen */
  for (i = 0; i < num_items; i++)
    putsxy(x+2, by + h * i, items[i]);

  /* update the screen */
//  rb->lcd_update();
}

/*
 * do_menu - draw menu on screen.
 *
 * Draw a menu titled @title on the screen, with @num_items elements
 * from @items, and select the @sel element.  If in doubt, set @sel to
 * -1 :).
 *
 */
static int do_menu(char *title, char **items, size_t num_items, int sel) {
    int btn, sel_item, ret, curr_item,y,x,c=0;
  bool done = false;
  ret = MENU_CANCEL;

 /*   for (y=0;y<144;y+=9) {
        for (x=0;x<160;x+=10) {
    fillRect(c, x, y, 10, 9);
c++;
}} while(1);*/ //palette!

  /* draw menu on screen and select the first item */
  draw_menu(title, items, num_items);
  curr_item = -1;
  select_item(title, curr_item, sel);
  curr_item = sel;

  /* make sure button state is empty */
  while (read_btn());
 //   rb->yield();

  /* loop until the menu is finished */
  while (!done) {
    /* grab a button */
    while (read_btn());
    btn = read_btn();
//printf("BTN : %x", btn);

    /* handle the button */
    if(btn & 0x08) {
        /* select next item in list */
        sel_item = curr_item + 1;
        if (sel_item >= (int) num_items)
          sel_item = 0;
        select_item(title, curr_item, sel_item);
        curr_item = sel_item;
   //     break;
      }
      else if(btn & 0x01) {
        /* select prev item in list */
        sel_item = curr_item - 1;
        if (sel_item < 0)
          sel_item = num_items - 1;
        select_item(title, curr_item, sel_item);
        curr_item = sel_item;
    //    break;
      }
      else if(btn & 0x04) {
        /* select current item */
        ret = curr_item;
        done = true;
    //    break;
      }
      else if(btn & 0x02) {
 for (y=0;y<144;y+=9) {
        for (x=0;x<160;x+=10) {
    fillRect(c, x, y, 10, 9);
c++;}}
      }
      else if(btn & 0x200) {
        /* cancel out of menu */
        ret = MENU_CANCEL;
        done = true;
   //     break;
      }
    
    
    /* give the OS some love */
 //   rb->yield();
  }
  
  /* return selected item */
  return ret;
}
