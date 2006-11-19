/*
*   kernel/init/main.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <api.h>
#include <evt.h>
#include <sys_def/font.h>
#include <sys_def/ctype.h>
#include <sys_def/colordef.h>
#include <sys_def/stddef.h>

int yDebug = 100;

//#define STATUS_LINE

#define WRAP_TRIM          44  /* Max number of spaces to trim (arbitrary) */
#define MAX_COLUMNS        64  /* Max displayable string len (over-estimate) */
#define MAX_WIDTH         910  /* Max line length in WIDE mode */
#define READ_PREV_ZONE    910  /* Arbitrary number less than SMALL_BLOCK_SIZE */
#define SMALL_BLOCK_SIZE  0x1000 /* 4k: Smallest file chunk we will read */
#define LARGE_BLOCK_SIZE  0x2000 /* 8k: Preferable size of file chunk to read */
#define BUFFER_SIZE       0x3000 /* 12k: Mem reserved for buffered file data */
#define TOP_SECTOR     buffer
#define MID_SECTOR     (buffer + SMALL_BLOCK_SIZE)
#define BOTTOM_SECTOR  (buffer + 2*(SMALL_BLOCK_SIZE))

/* Out-Of-Bounds test for any pointer to data in the buffer */
#define BUFFER_OOB(p)    ((p) < buffer || (p) >= buffer_end)

/* Does the buffer contain the beginning of the file? */
#define BUFFER_BOF()     (file_pos==0)

/* Does the buffer contain the end of the file? */
#define BUFFER_EOF()     (file_size-file_pos <= BUFFER_SIZE)

/* Formula for the endpoint address outside of buffer data */
#define BUFFER_END() \
 ((BUFFER_EOF()) ? (file_size-file_pos+buffer) : (buffer+BUFFER_SIZE))

/* Is the entire file being shown in one screen? */
#define ONE_SCREEN_FITS_ALL() \
 (next_screen_ptr==NULL && screen_top_ptr==buffer && BUFFER_BOF())

/* Is a scrollbar called for on the current screen? */
#define NEED_SCROLLBAR() ((!(ONE_SCREEN_FITS_ALL())) && \
 (view_mode==WIDE? scrollbar_mode[WIDE]==SB_ON: scrollbar_mode[NARROW]==SB_ON))

enum {
    WRAP=0,      
    CHOP,
    WORD_MODES
} word_mode = 0;

enum {
    NORMAL=0,
    JOIN,
    EXPAND,
    LINE_MODES
} line_mode = 0;

enum {
    NARROW=0,
    WIDE,
    VIEW_MODES
} view_mode = 0;

enum {
    SB_OFF=0,
    SB_ON,
    SCROLLBAR_MODES
} scrollbar_mode[VIEW_MODES] = {SB_ON, SB_ON};

static int need_scrollbar;
enum {
    NO_OVERLAP=0,
    OVERLAP,
    PAGE_MODES
} page_mode = 0;

#ifdef STATUS_LINE
static unsigned char *word_mode_str[] = {"wrap", "chop", "words"};
static unsigned char *line_mode_str[] = {"normal", "join", "expand", "lines"};
static unsigned char *view_mode_str[] = {"narrow", "wide", "view"};
#endif

static unsigned char buffer[BUFFER_SIZE + 1];
static unsigned char line_break[] = {0,0x20,'-',9,0xB,0xC};
static int display_columns; /* number of columns on the display */
static int display_lines; /* number of lines on the display */
static int fd;
static long file_size;
static int mac_text;
static long file_pos; /* Position of the top of the buffer in the file */
static unsigned char *buffer_end; /*Set to BUFFER_END() when file_pos changes*/
static int max_line_len;
static unsigned char *screen_top_ptr;
static unsigned char *next_screen_ptr;
static unsigned char *next_screen_to_draw_ptr;
static unsigned char *next_line_ptr;
static int screen_height,screen_width;
static int font_height,font_width;

WIDGETMENU menu;

char * font_items[]={"4x6","5x8","6x9","7x13","8x13"};
int font_ids[]={STD4X6,STD5X8,STD6X9,STD7X13,STD8X13};

char * word_items[]={"Wrap","Chop"};
char * line_items[]={"Normal","Join","Expand"};

void DrawStatusLine(int type)
{
#ifdef STATUS_LINE
   char tmp[50];

   sprintf(tmp, "%s %s / %s %s / %s %s",  word_mode_str[word_mode],
	word_mode_str[WORD_MODES],
	line_mode_str[line_mode],
	line_mode_str[LINE_MODES],
	view_mode_str[view_mode],
	view_mode_str[VIEW_MODES]);

	gfx_fillRect(COLOR_BLUE, 0, screen_width, screen_height, 16);
	gfx_putS(COLOR_BLACK, COLOR_BLUE, 5, display_lines * font_height, tmp);
#endif
}

static unsigned char* find_first_feed(const unsigned char* p, int size)
{
    int i;

    for (i=0; i < size; i++)
        if (p[i] == 0)
            return (unsigned char*) p+i;

    return NULL;
}

static unsigned char* find_last_feed(const unsigned char* p, int size)
{
    int i;

    for (i=size-1; i>=0; i--)
        if (p[i] == 0)
            return (unsigned char*) p+i;

    return NULL;
}

static unsigned char* find_last_space(const unsigned char* p, int size)
{
    int i, j, k;

    k = line_mode==JOIN? 0:1;

    for (i=size-1; i>=0; i--)
        for (j=k; j < (int) sizeof(line_break); j++)
            if (p[i] == line_break[j])
                return (unsigned char*) p+i;

    return NULL;
}

static unsigned char* find_next_line(const unsigned char* cur_line)
{
    const unsigned char *next_line = NULL;
    int size, i, j, k, chop_len, search_len, spaces, newlines, draw_columns;
    unsigned char c;


    if BUFFER_OOB(cur_line)
        return NULL;

    draw_columns = need_scrollbar? display_columns-1: display_columns;

    if (view_mode == WIDE) {
        search_len = chop_len = MAX_WIDTH;
	 }
    else {   /* view_mode == NARROW */
        chop_len = draw_columns;
        search_len =  chop_len + 1;
    }


    size = BUFFER_OOB(cur_line+search_len) ? buffer_end-cur_line : search_len;


    if (line_mode == JOIN) {
        /* Need to scan ahead and possibly increase search_len and size,
         or possibly set next_line at second hard return in a row. */

        next_line = NULL;
        for (j=k=spaces=newlines=0; j < size; j++) {
            if (k == MAX_COLUMNS)
                break;

            c = cur_line[j];
            switch (c) {
                case ' ':
                    spaces++;
                    break;

                case 0:
                    if (newlines > 0)
						  {
                        size = j;
                        next_line = cur_line + size - spaces - 1;
                        if (next_line != cur_line)
                            return (unsigned char*) next_line;
                        break;
                    }
                    newlines++;
                    size += spaces;
                    if (BUFFER_OOB(cur_line+size) || size > 2*search_len)
                        return NULL;

                    search_len = size;
                    spaces = 0;
                    k++;
                    break;

                default:
                    newlines = 0;
                    while (spaces) {
                        spaces--;
                        k++;
                        if (k == MAX_COLUMNS - 1)
                            break;
                    }
                    k++;
                    break;
            }
        }
    }
    else {
        /* find first hard return */
//        lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 5, 210, "first_feed");
        next_line = find_first_feed(cur_line, size);
    }

    if (next_line == NULL)
//        sprintf(tmp,"%ld %ld %ld", size,search_len, display_columns);
//        lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 170, 210, tmp);
        if (size == search_len)
		  {
            if (word_mode == WRAP)  /* Find last space */
				{
                next_line = find_last_space(cur_line, size);
				}

            if (next_line == NULL)
				{
                next_line = cur_line + chop_len;
//                lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 50, 80, next_line);
//                lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 50, 100, cur_line);
//                sprintf(tmp,"%d", chop_len);
//                lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 50, 120, tmp);
				}
            else
				{
                if (word_mode == WRAP)
					 {
                    for (i=0; i<WRAP_TRIM && isspace(next_line[0]) && !BUFFER_OOB(next_line); i++)
                        next_line++;
					 }
			   }
        }

    if (line_mode == EXPAND)
	 {
        if (!BUFFER_OOB(next_line))  /* Not Null & not out of bounds */
            if (next_line[0] == 0)
                if (next_line != cur_line)
                    return (unsigned char*) next_line;
	 }

    /* If next_line is pointing to a zero, increment it; i.e.,
     leave the terminator at the end of cur_line. If pointing
     to a hyphen, increment only if there is room to display
     the hyphen on current line (won't apply in WIDE mode,
     since it's guarenteed there won't be room). */
    if (!BUFFER_OOB(next_line))  /* Not Null & not out of bounds */
	 {
//        lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 60, yDebug, "x");
// 		  yDebug+=10;

        if (next_line[0] == 0 || (next_line[0] == '-' && next_line-cur_line < draw_columns))
		  {
//            lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 80, yDebug, "y");
//				yDebug+=10;
            next_line++;
		  }
	 }

    if (BUFFER_OOB(next_line))
	 {
//        lcd_putsxy(COLOR_BLACK, COLOR_WHITE, 100, yDebug, "z");
//		  yDebug+=10;
        return NULL;
	 }

    return (unsigned char*) next_line;
}

static unsigned char* find_prev_line(const unsigned char* cur_line)
{
    const unsigned char *prev_line = NULL;
    const unsigned char *p;

    if BUFFER_OOB(cur_line)
        return NULL;

    /* To wrap consistently at the same places, we must
     start with a known hard return, then work downwards.
     We can either search backwards for a hard return,
     or simply start wrapping downwards from top of buffer.
       If current line is not near top of buffer, this is
     a file with long lines (paragraphs). We would need to
     read earlier sectors before we could decide how to
     properly wrap the lines above the current line, but
     it probably is not worth the disk access. Instead,
     start with top of buffer and wrap down from there.
     This may result in some lines wrapping at different
     points from where they wrap when scrolling down.
       If buffer is at top of file, start at top of buffer. */

    if (line_mode == JOIN)
        prev_line = p = NULL;
    else
        prev_line = p = find_last_feed(buffer, cur_line-buffer-1);
        /* Null means no line feeds in buffer above current line. */

    if (prev_line == NULL)
        if (BUFFER_BOF() || cur_line - buffer > READ_PREV_ZONE)
            prev_line = p = buffer;
        /* (else return NULL and read previous block) */

    /* Wrap downwards until too far, then use the one before. */
    while (p < cur_line && p != NULL) {
        prev_line = p;
        p = find_next_line(prev_line);
    }

    if (BUFFER_OOB(prev_line))
        return NULL;

    return (unsigned char*) prev_line;
}

static void fill_buffer(long pos, unsigned char* buf, unsigned size)
{
    /* Read from file and preprocess the data */
    /* To minimize disk access, always read on sector boundaries */
    unsigned numread, i;
    int found_CR = 0;
    lseek(fd, pos, SEEK_SET);
    numread = read(fd, buf, size);


	 for(i = 0; i < numread; i++)
	 {
        switch(buf[i]) {
            case '\r':
                if (mac_text) {
                    buf[i] = 0;
                }
                else {
                    buf[i] = ' ';
                    found_CR = 1;
                }
                break;

            case '\n':
                buf[i] = 0;
                found_CR = 0;
                break;

            case 0:  /* No break between case 0 and default, intentionally */
                buf[i] = ' ';
            default:
                if (found_CR) {
                    buf[i - 1] = 0;
                    found_CR = 0;
                    mac_text = 1;
                }
                break;
        }
    }
}

static int read_and_synch(int direction)
{
/* Read next (or prev) block, and reposition global pointers. */
/* direction: 1 for down (i.e., further into file), -1 for up */
    int move_size, move_vector, offset;
    unsigned char *fill_buf;

    if (direction == -1) /* up */ {
        move_size = SMALL_BLOCK_SIZE;
        offset = 0;
        fill_buf = TOP_SECTOR;
        memcpy(BOTTOM_SECTOR, MID_SECTOR, SMALL_BLOCK_SIZE);
        memcpy(MID_SECTOR, TOP_SECTOR, SMALL_BLOCK_SIZE);
    }
    else /* down */ {
        if (view_mode == WIDE) {
            /* WIDE mode needs more buffer so we have to read smaller blocks */
            move_size = SMALL_BLOCK_SIZE;
            offset = LARGE_BLOCK_SIZE;
            fill_buf = BOTTOM_SECTOR;
            memcpy(TOP_SECTOR, MID_SECTOR, SMALL_BLOCK_SIZE);
            memcpy(MID_SECTOR, BOTTOM_SECTOR, SMALL_BLOCK_SIZE);
        }
        else {
            move_size = LARGE_BLOCK_SIZE;
            offset = SMALL_BLOCK_SIZE;
            fill_buf = MID_SECTOR;
            memcpy(TOP_SECTOR, BOTTOM_SECTOR, SMALL_BLOCK_SIZE);
        }
    }
    move_vector = direction * move_size;
    screen_top_ptr -= move_vector;
    file_pos += move_vector;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */
    fill_buffer(file_pos + offset, fill_buf, move_size);
    return move_vector;
}

static void viewer_scroll_up(void)
{
    unsigned char *p;

    p = find_prev_line(screen_top_ptr);
    if (p == NULL && !BUFFER_BOF()) {
        read_and_synch(-1);
        p = find_prev_line(screen_top_ptr);
    }
    if (p != NULL)
        screen_top_ptr = p;
}

static void viewer_scrollbar(void)
{

  int items, min_shown, max_shown;


    items = (int) file_size;  
    min_shown = (int) file_pos + (screen_top_ptr - buffer);

    if (next_screen_ptr == NULL)
        max_shown = items;
    else
        max_shown = min_shown + (next_screen_ptr - screen_top_ptr);
        
    printf("i %d min %d max %d\n",items,min_shown,max_shown);
}

static void viewer_draw(int col)
{
    int i, j, k, line_len, resynch_move, spaces, left_col=0;
    unsigned char *line_begin;
    unsigned char *line_end;
    unsigned char c;
    unsigned char scratch_buffer[MAX_COLUMNS + 1];
    char text[200];

    /* If col==-1 do all calculations but don't display */
    if (col != -1)
	 {
        gfx_fillRect(COLOR_WHITE, 0, 0, screen_width, screen_height);
    }

    max_line_len = 0;
    line_begin = line_end = screen_top_ptr;
    for (i = 0; i < display_lines; i++) {
        if (BUFFER_OOB(line_end))
		  {
            break;  /* Happens after display last line at BUFFER_EOF() */
        }

        line_begin = line_end;
        line_end = find_next_line(line_begin);


        if (line_end == NULL)
		  {

            if (BUFFER_EOF())
				{
                if (i < display_lines - 1 && !BUFFER_BOF())
					 {
                    if (col != -1)
						  {
                       gfx_fillRect(COLOR_WHITE, 0, 0, screen_width, screen_height);
						  }

                    for (; i < display_lines - 1; i++)
                        viewer_scroll_up();

                    line_begin = line_end = screen_top_ptr;
                    i = -1;
                    continue;
                }
                else {
                    line_end = buffer_end;
                }
            }
            else
				{

                resynch_move = read_and_synch(1); /* Read block & move ptrs */
                line_begin -= resynch_move;
                if (i > 0)
                    next_line_ptr -= resynch_move;

                line_end = find_next_line(line_begin);
                if (line_end == NULL)  /* Should not really happen */
                    break;
            }
        }
        line_len = line_end - line_begin;
        if (line_mode == JOIN)
		  {
            if (line_begin[0] == 0) {
                line_begin++;
                if (word_mode == CHOP)
                    line_end++;
            }
            for (j=k=spaces=0; j < line_len; j++) {
                if (k == MAX_COLUMNS)
                    break;

                c = line_begin[j];
                switch (c) {
                    case ' ':
                        spaces++;
                        break;
                    case 0:
                        spaces = 0;
                        scratch_buffer[k++] = ' ';
                        break;
                    default:
                        while (spaces) {
                            spaces--;
                            scratch_buffer[k++] = ' ';
                            if (k == MAX_COLUMNS - 1)
                                break;
                        }
                        scratch_buffer[k++] = c;
                        break;
                }
            }
            if (col != -1)
                if (k > col)
					 {
                    scratch_buffer[k] = 0;

						  memset(text, 0, sizeof(text));
						  strncpy(text,scratch_buffer + col, k);
                    gfx_putS(COLOR_BLACK, COLOR_WHITE, left_col*font_width, i*font_height, text);

                }
        }
        else
		  {
            if (col != -1)
				{
                if (line_len > col)
					 {
                    c = line_end[0];
                    line_end[0] = 0;

						  memset(text, 0, sizeof(text));
						  strncpy(text, line_begin + col, line_len);
                    gfx_putS(COLOR_BLACK, COLOR_WHITE, left_col*font_width, i*font_height, text);

                    line_end[0] = c;
                }
			   }
        }
        if (line_len > max_line_len)
		  {
            max_line_len = line_len;
		  }

        if (i == 0)
            next_line_ptr = line_end;
    }

	 next_screen_ptr = line_end;

    if (BUFFER_OOB(next_screen_ptr))
        next_screen_ptr = NULL;

    next_screen_to_draw_ptr = page_mode==OVERLAP? line_begin: next_screen_ptr;

    if (need_scrollbar)
        viewer_scrollbar();

}

static void viewer_top(void)
{

    /* Read top of file into buffer
      and point screen pointer to top */
    file_pos = 0;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */
    screen_top_ptr = buffer;
    fill_buffer(0, buffer, BUFFER_SIZE);
}

static void viewer_bottom(void)
{
    /* Read bottom of file into buffer
      and point screen pointer to bottom */
    long last_sectors;

    if (file_size > BUFFER_SIZE) {
        /* Find last buffer in file, round up to next sector boundary */
        last_sectors = file_size - BUFFER_SIZE + SMALL_BLOCK_SIZE;
        last_sectors /= SMALL_BLOCK_SIZE;
        last_sectors *= SMALL_BLOCK_SIZE;
    }
    else {
        last_sectors = 0;
    }
    file_pos = last_sectors;
    buffer_end = BUFFER_END();  /* Update whenever file_pos changes */
    screen_top_ptr = buffer_end-1;
    fill_buffer(last_sectors, buffer, BUFFER_SIZE);
}

static void init_need_scrollbar(void) {
    /* Call viewer_draw in quiet mode to initialize next_screen_ptr,
     and thus ONE_SCREEN_FITS_ALL(), and thus NEED_SCROLLBAR() */
    viewer_draw(-1);
    need_scrollbar = NEED_SCROLLBAR();
}

static int viewer_init(char* file)
{
    /*********************
    * (Could re-initialize settings here, if you
    *   wanted viewer to start the same way every time)*/
    word_mode = WRAP;
    line_mode = NORMAL;
    view_mode = NARROW;
    page_mode = NO_OVERLAP;
    scrollbar_mode[NARROW] = SB_OFF;
    scrollbar_mode[WIDE] = SB_ON;

    fd = open(file, O_RDONLY);
    if (fd<0)
    {
        printf("Error, can't open file\n");
        return 0;
    }

    lseek(fd, 0, SEEK_END);
    file_size = lseek(fd, 0, SEEK_CUR);
    if (file_size==-1)
        return 0;

    /* Init mac_text value used in processing buffer */
    mac_text = 0;

    /* Read top of file into buffer;
      init file_pos, buffer_end, screen_top_ptr */
    viewer_top();

    /* Init need_scrollbar value */
    init_need_scrollbar();

    return 1;
}

void font_set(int font){
    gfx_fontSet(font);

    gfx_getStringSize("M", &font_width, &font_height);

    display_lines = (screen_height / font_height);
#ifdef STATUS_LINE
    display_lines--;  // reserve status line
#endif
    display_columns = screen_width / font_width ;
}

static void viewer_exit(void)
{
    if(fd<0)
        return;
    close(fd);
}

static int col_limit(int col)
{
    if (col < 0)
        col = 0;
    else
        if (col > max_line_len - 2)
            col = max_line_len - 2;

    return col;
}

void menu_init(){
    WIDGETMENU_CHOOSER mih;
    WIDGETMENU_CHECKBOX mic;

    menu=widgetMenu_create();
    menu->setRect(menu,0,0,screen_width,screen_height);
    menu->ownItems=true;

    mih=widgetMenuChooser_create();
    mih->caption="Font";
    mih->chooser->items=font_items;
    mih->chooser->itemCount=5;
    mih->chooser->index=3;
    menu->addItem(menu,mih);

    mic=widgetMenuCheckbox_create();
    mic->caption="Line wrap";
    mic->checkbox->caption="Wrap";
    mic->checkbox->checked=true;
    menu->addItem(menu,mic);

    mih=widgetMenuChooser_create();
    mih->caption="Words";
    mih->chooser->items=word_items;
    mih->chooser->itemCount=2;
    mih->chooser->index=0;
    menu->addItem(menu,mih);

    mih=widgetMenuChooser_create();
    mih->caption="Lines";
    mih->chooser->items=line_items;
    mih->chooser->itemCount=3;
    mih->chooser->index=0;
    menu->addItem(menu,mih);
}

void menu_execute(int evt_handler){
    bool wrap;
    int font;
    int event;

    menu->handleEvent(menu,EVT_REDRAW);

    do
    {
        event=evt_getStatus(evt_handler);
        if (!event) continue; // no new events

        menu->handleEvent(menu,event);
    }
    while(event!=BTN_OFF);

    font=menu->getChooser(menu,menu->indexFromCaption(menu,"Font"))->index;
    word_mode=menu->getChooser(menu,menu->indexFromCaption(menu,"Words"))->index;
    line_mode=menu->getChooser(menu,menu->indexFromCaption(menu,"Lines"))->index;
    wrap=menu->getCheckbox(menu,menu->indexFromCaption(menu,"Line wrap"))->checked;
    view_mode=(wrap)?NARROW:WIDE;
    font_set(font_ids[font]);
}

void app_main(int argc,char ** argv)
{
    char *file;
    int ok;
	int col = 0;
	int evt;
	int stop=0;
	int i = 0;
	int evt_handler = evt_getHandler(BTN_CLASS);
    
    if(argc<2)
        return ; // Quit
    else
    {
        file=argv[1];
    }


	gfx_openGraphics();

	getResolution(&screen_width,&screen_height);

    // set sandard font
    font_set(STD6X9);

    menu_init();

    ok = viewer_init(file);

    if (!ok)
    {
        viewer_exit();
        return ;
    }

    viewer_draw(col);
    DrawStatusLine(0);

    while (!stop)
	 {
        while((evt=evt_getStatus(evt_handler))>0)
        {
            switch(evt)
            {
                case BTN_OFF:
                        viewer_exit();
                        stop = 1;
                        break;

                case BTN_UP:
                        /* Page up */
                        for (i = page_mode==OVERLAP? 1:0; i < display_lines; i++)
                                viewer_scroll_up();
                
                        viewer_draw(col);
                        DrawStatusLine(0);
                        break;
                
                case BTN_DOWN:
                        /* Page down */
                        if (next_screen_ptr != NULL)
                                screen_top_ptr = next_screen_to_draw_ptr;
                
                        viewer_draw(col);
                        DrawStatusLine(0);
                        break;
                
                case BTN_LEFT:
                        if (view_mode == WIDE) {
                                /* Screen left */
                                col -= display_columns;
                                col = col_limit(col);
                        }
                        else {   /* view_mode == NARROW */
                                /* Top of file */
                                viewer_top();
                        }
                
                        viewer_draw(col);
                        DrawStatusLine(0);
                        break;
                
                case BTN_RIGHT:
                        if (view_mode == WIDE) {
                                /* Screen right */
                                col += display_columns;
                                col = col_limit(col);
                        }
                        else {   /* view_mode == NARROW */
                                /* Bottom of file */
                                viewer_bottom();
                        }
                
                        viewer_draw(col);
                        DrawStatusLine(0);
                        break;

#ifdef STATUS_LINE
                case BTN_F1:
                        /* Word-wrap mode: WRAP or CHOP */
                        if (++word_mode == WORD_MODES)
                                word_mode = 0;

                        init_need_scrollbar();

                        viewer_draw(col);

                        DrawStatusLine(1);
                
                        break;
                
                case BTN_F2:
                        /* Line-paragraph mode: NORMAL, JOIN or EXPAND */
                        if (++line_mode == LINE_MODES)
                                line_mode = 0;
                
                        if (view_mode == WIDE)
                                if (line_mode == JOIN)
                                                if (++line_mode == LINE_MODES)
                                                        line_mode = 0;
                
                        init_need_scrollbar();
                
                        viewer_draw(col);
                
                        DrawStatusLine(2);
                
                        break;
                
                case BTN_F3:
                        /* View-width mode: NARROW or WIDE */
                        if (line_mode == JOIN)
                        {
                                DrawStatusLine(3);
                        }
                        else
                                if (++view_mode == VIEW_MODES)
                                                view_mode = 0;

                        col = 0;

                        /***** Could do this after change of word-wrap mode
                        * and after change of view-width mode, to normalize
                        * view:
                        if (screen_top_ptr > buffer + BUFFER_SIZE/2) {
                                screen_top_ptr = find_prev_line(screen_top_ptr);
                                screen_top_ptr = find_next_line(screen_top_ptr);
                        }
                        else {
                                screen_top_ptr = find_next_line(screen_top_ptr);
                                screen_top_ptr = find_prev_line(screen_top_ptr);
                        }
                        ***********/

                        init_need_scrollbar();

                        viewer_draw(col);

                        DrawStatusLine(4);

                        break;
#else
                case BTN_F1:
                case BTN_F2:
                case BTN_F3:
#endif
                case BTN_ON:
                        menu_execute(evt_handler);
                        viewer_draw(col);
                        break;
            }
        }
    }
    evt_freeHandler(evt_handler);
}

