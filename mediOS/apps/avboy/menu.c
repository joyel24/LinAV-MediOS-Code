/*********************************************************************/
/* menu.c - user menu for rockboy                                    */
/*                                                                   */
/* Modified by CjNr11 07/12/2005                                     */
/*********************************************************************/

#include <sys_def/ctype.h>
#include <sys_def/string.h>
#include "mem.h"
#include <fs_io.h>
#include <graphics.h>
#include <kernel/malloc.h>
#include <kernel/buttons.h>

#include "avboy.h"

#define getstringsize(a,b,c) getStringS(a,b,c)
#define drawline(a,b,c,d,e) drawLine(e,a,b,c,d)
#define drawrect(a,b,c,d,e) drawRect(e,a,b,c,d)
#define fillrect(a,b,c,d,e) fillRect(e,a,b,c,d)
#define putsxy(a,b,c) putS(0xFF,0x00,a,b,c)

 
#define OSD_BITMAP1_WIDTH 160
#define OSD_BITMAP1_HEIGHT 144

#define USER_MENU_QUIT -2

/* load/save state function declarations */
static void do_slot_menu(bool is_load);
static void do_opt_menu(void);
static void munge_name(char *buf, size_t bufsiz);

/* directory ROM save slots belong in */
#define STATE_DIR "/avboy/states"

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
//  SM_ITEM_FILE,
  SM_ITEM_BACK,
  SM_ITEM_LAST
} SlotMenuItem;


/* this semi-evil, but we snprintf() into these strings later
 * Note: if you want more save slots, just add more lines 
 * to this array */
static const char *slot_menu[] = {
  "1.              ",
  "2.              ",
  "3.              ",
  "4.              ",
  "5.              ",
//  "Save to File...    ",
  "Previous Menu..."
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
  if(max > bufsiz) max =  bufsiz;
  
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
 // char name_buf[40];
  char *name_buf;
  name_buf=(char *)bget(40);
  /* munge state file name */
  strncpy(name_buf, rom.name,256); // sizeof(name_buf));
  name_buf[16] = '\0';
  munge_name(name_buf, strlen(name_buf));

  /* glom the whole mess together */
  snprintf(buf, bufsiz, "%s/%s-%d.avb", STATE_DIR, name_buf, slot_id + 1);
  brel(name_buf);
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
static bool do_file(char *path/*, char *desc*/, bool is_load) {
  int fd; //, file_mode;

  /* load/save state */
  if (is_load) {
    fd = fopen(path, O_RDONLY );
    if(!fd) {
      printf("Retry...\n"); 
      fd = fopen(path, O_RDONLY );
      if(!fd) return false;
      else printf("File opened!\n");
    }
    else printf("File opened!\n");

    /* load state */
    loadstate(fd);
    printf("Loaded state from \"%s\"\n", path);

  } else {
    fd = fopen(path, O_WRONLY | O_CREAT );
    if(!fd) {
      printf("Retry...\n"); 
      fd = fopen(path, O_WRONLY | O_CREAT ); 
      if(!fd) return false;
      else printf("File opened!\n");
    }
    else printf("File opened!\n");

    /* save state */
    savestate(fd);
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
  char *path_buf;
  bool res;
  path_buf=(char *)bget(256);

  /* build slot filename*/
  build_slot_path(path_buf, 256, slot_id);
  printf("Path : %s\n",path_buf);

  /* load/save file */
  res = do_file(path_buf/*, desc_buf*/, is_load);
  brel(path_buf);
  return  res;
}

/*
 * get information on the given slot
 */
static void slot_info(char *info_buf, size_t info_bufsiz, size_t slot_id) {
  char * buf;
  int fd;
  buf=(char *)bget(256);

  /* get slot file path */
  build_slot_path(buf, 256, slot_id);

  /* attempt to open slot */
  if ((fd = fopen(buf, O_RDONLY)) >= 0) {
    snprintf(info_buf, info_bufsiz, "%2d. State Saved", slot_id + 1);
    fclose(fd);
  } else {
    /* if we couldn't open the file, then the slot is empty */
    snprintf(info_buf, info_bufsiz, "%2d. Empty", slot_id + 1);
    brel(buf);
  }
}

/*
 * do_slot_menu - prompt the user for a load/save memory slot
 */
static void do_slot_menu(bool is_load) {
  int i, mi, ret, num_items;
  bool done = false;
  char *title;

  /* set defaults */
  ret = 0; /* return value */
  mi = 0; /* initial menu selection */
  num_items = sizeof(slot_menu) / sizeof(char*);
  
  /* create menu items (the last two are file and previous menu,
   * so don't populate those) */
  for (i = 0; i < num_items - 1; i++)
    slot_info((char*) slot_menu[i], 20, i);

  /* set menu title */
  if(is_load) title = "Load State";
  else title = "Save State";

  /* loop until we should exit menu */
  while (!done) {
    /* get item selection */
    mi = do_menu(title, (char**) slot_menu, num_items, mi);

    /* handle selected menu item */
    done = true;
    if (mi != MENU_CANCEL && mi != SM_ITEM_BACK) {
      done = do_slot(mi, is_load);

      /* if we couldn't save the state file, then print out an
       * error message */
      if (!is_load && !done)
      printf("Couldn't save state file.");
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

  /* if there is a current item, then deselect it */
  if (curr_item >= 0) {
    /* deselect old item */
    y = MENU_Y + h + MENU_ITEM_PAD * 2; /* account for title */
    y += h * curr_item;
    drawrect(x, y, w, h,0x00);
  }

  /* select new item */
  curr_item = item_i;

  /* select new item */
  y = MENU_Y + h + MENU_ITEM_PAD * 2; /* account for title */
  y += h * curr_item;
  drawrect(x, y, w, h,0xf9);
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

  /* draw the outline */
  fillRect(0xaf,SHADOW_RECT);
  fillRect(0x00,MENU_RECT);
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
  fillrect((OSD_BITMAP1_WIDTH - w) / 2 - 2, y - 2, w + 4, h,0x00);

  /* draw centered title on screen */
  putsxy((OSD_BITMAP1_WIDTH - w)/2, y, title);
  
  /* calculate base Y for items */
  by = y + h + MENU_ITEM_PAD;
  
  /* iterate over each item and draw it on the screen */
  for (i = 0; i < num_items; i++)
    putsxy(x+2, by + h * i, items[i]);
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


  /* draw menu on screen and select the first item */
  draw_menu(title, items, num_items);
  curr_item = -1;
  select_item(title, curr_item, sel);
  curr_item = sel;

  /* make sure button state is empty */
  while (read_btn());

  /* loop until the menu is finished */
  while (!done) {
    /* grab a button */
    while (read_btn());
    btn = read_btn();

    /* handle the button */
    if(btn & 0x08) {
        /* select next item in list */
        sel_item = curr_item + 1;
        if (sel_item >= (int) num_items)
          sel_item = 0;
        select_item(title, curr_item, sel_item);
        curr_item = sel_item;
      }
      else if(btn & 0x01) {
        /* select prev item in list */
        sel_item = curr_item - 1;
        if (sel_item < 0)
          sel_item = num_items - 1;
        select_item(title, curr_item, sel_item);
        curr_item = sel_item;
      }
      else if(btn & 0x04) {
        /* select current item */
        ret = curr_item;
        done = true;
      }
      else if(btn & 0x02 && btn & 0x100) {
        for (y=0;y<144;y+=9) {
          for (x=0;x<160;x+=10) {
            fillRect(c, x, y, 10, 9);
            c++;
          }
        }
      }
      else if(btn & 0x200) {
        /* cancel out of menu */
        ret = MENU_CANCEL;
        done = true;
      }
//      else if(btn & 0x100) {
//      }
  }

  /* return selected item */
  return ret;
}

void browser(char * rom) {
  int x, y, w, h, by;
  size_t i;

  int btn, sel_item=0, curr_item, num_items=6, nb=0,pos=0;
  char title[]="Start...";
  char (*items)[MAX_PATH];
  char (*list)[MAX_PATH];
  bool done = false;
  struct dirent *romdir;
  DIR * romd=NULL;

items = bget(MAX_PATH*6);
list = bget(MAX_PATH);

romd=opendir("/avboy/roms");
if(romd) printf("Dir /avboy/roms/ opened!\n");
else printf("Dir error!\n");


  while((romdir=readdir(romd))!=NULL) {
     if(!(romdir->attribute & ATTR_DIRECTORY)) {
     list[nb][0]='\0';
     strcat(list[nb],romdir->d_name);
     nb++;
     list = bgetr(list,MAX_PATH*(nb+1));
     }
  }

  fillRect(0xaf,11,9,140,128);
  fillRect(0x00,10,8,140,128);
  drawRect(0xff,10,8,140,128);

  x = 10 + 2;
  y = 8 + 2 * 2;
  getstringsize(title, &w, &h);
  h += 2 * 2;

  for (i = 8; i < (size_t) y + h; i += 2)
    drawline(10, i, 10 + 139, i,0xff);

  fillrect((160 - w) / 2 - 2, y - 2, w + 4, h,0x00);
  putsxy((160 - w)/2, y, title);
  
  by = y + h + 2;
  


while(!done) {
  if(pos > (nb-6)) num_items=nb-pos;
  else num_items=6;
  for(i=0;i<num_items;i++) {items[i][0]='\0'; strcat(items[i],list[i+pos]);}

  fillRect(0x00,11,29,138,106);
  for (i = 0; i < num_items; i++)
    putsxy(x+2, by + h * i, items[i]);

  curr_item = -1;
  select_item(title, curr_item, sel_item);
  curr_item = sel_item;

  while (1) {
    while (read_btn());
    btn = read_btn();

    if(btn & 0x08) {
        sel_item = curr_item + 1;
        if (sel_item >= (int) num_items) {
           if(pos < (nb-6)) {sel_item=0;pos+=6;break;}
           else {sel_item = curr_item;}
        }
        else {
           select_item(title, curr_item, sel_item);
           curr_item = sel_item;
        }
      }
      else if(btn & 0x01) {
        sel_item = curr_item - 1;
        if (sel_item < 0) {
           if(pos > 0) {sel_item=5;pos-=6;break;}
           else {sel_item = curr_item;}
        }
        else {
           select_item(title, curr_item, sel_item);
           curr_item = sel_item;
        }
      }
      else if(btn & 0x04) {
        done = true;
        break;
      }
      else if(btn & 0x02) {
      }

  }
  }

  rom[0]='\0';
  strcat(rom,"/AVBOY/ROMS/");
  strcat(rom,items[curr_item]);
  strcat(rom,"\0");
  brel(items);
  brel(list);
  return;
}


