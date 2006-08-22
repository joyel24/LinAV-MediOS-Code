/*
*   mastermind.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* initial code from Schoki - Linav project 2004
* 
*
*/

#include <api.h>
#include <evt.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#define PIECE_DIM 12

void displayMineField(void);


/***************************************
*
* Warning some coordinate are still hardcoded: for menu, and other texts
*
****************************************/


int GameMode = 0;

int tiles_left = 0;
int g_changes = 0;

/* percentage of mines on minefield used durring generation */
int p=22;

/* the cursor coordinates */
int x=0,y=0;

/* define how numbers are displayed (that way we don't have to */
/* worry about fonts) */

static unsigned char emptyField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char oneField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char twoField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char threeField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char fourField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char fiveField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char sixField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char sevenField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  9,  9,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static unsigned char eightField[PIECE_DIM][PIECE_DIM] =
    { { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  9,  7,  7,  9,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2} };

static BITMAP emptyB = {(unsigned int) emptyField, PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP oneB   = {(unsigned int) oneField,   PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP twoB   = {(unsigned int) twoField,   PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP threeB = {(unsigned int) threeField, PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP fourB  = {(unsigned int) fourField,  PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP fiveB  = {(unsigned int) fiveField,  PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP sixB   = {(unsigned int) sixField,   PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP sevenB = {(unsigned int) sevenField, PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP eightB = {(unsigned int) eightField, PIECE_DIM, PIECE_DIM, 0, 0};

/* the tile struct
if there is a mine, mine is true
if tile is known by player, known is true
if tile has a flag, flag is true
neighbors is the total number of mines arround tile
*/
typedef struct tile_s {
    unsigned char mine : 1;
    unsigned char known : 1;
    unsigned char flag : 1;
    unsigned char neighbors : 4;
} tile;

/* the height and width of the field */
/* could be variable if malloc worked in the API :) */
int height;
int width ;
int screen_height;
int screen_width;

#define MINE_FIELD(I,J) minefield[(I)*height+(J)]

int settingParam = 0;

/* the minefield */
tile * minefield;//[screen_height/PIECE_DIM][(screen_width-50)/PIECE_DIM];

/* total number of mines on the game */
int mine_num = 0;

void writeMinesINI()
{
#warning need a cfg writer system
#if 0
    char tmp[250];

    if(g_changes == 1)
    {
        openCfg("/mnt/avwm/plugins/minesweeper.ini",CFG_WRITE);

        sprintf(tmp,"%d",p);
        putCfg("mines",tmp);
        newLine();
        sprintf(tmp,"%d",height);
        putCfg("heigth",tmp);
        newLine();
        sprintf(tmp,"%d",width);
        putCfg("width",tmp);
        newLine();

        closeCfg();

        g_changes = 0;
    }
#endif
}

void readMinesINI()
{
#warning need a cfg reader system
#if 0
    char item_buff[MAX_TOKEN+1];
    char value_buff[MAX_TOKEN+1];
    char *item=item_buff;
    char *value=value_buff;

    if(openCfg("/mnt/avwm/plugins/minesweeper.ini",CFG_READ)<0)
        return;

    while (1)
    {
        /* get next item/value couple, returns 0 if ther is no more item/value in the file */
        if (!getCfg(item,value)) break;
        /* now parse the item using strcmp */
        if(!strcmp(item,"mines"))
        {
            p=atoi(value);
        }
        else if(!strcmp(item,"heigth"))
        {
            height=atoi(value);
        }
        else if(!strcmp(item,"width"))
        {
            width=atoi(value);
        }
        else
            printf("unknown item type: %s on line %d\n",item,curLineNum());
    }

    closeCfg();
#endif
}

void RefreshSettings()
{
    char tmp[10];
    gfx_fillRect(COLOR_GREEN, 0, 20, 160,80); // clear

    sprintf(tmp,"%d", p);

    if(settingParam == 0)
    {
        sprintf(tmp,"%d", width);
        gfx_putS(COLOR_RED, COLOR_GREEN, 5,30, "Width");
        gfx_putS(COLOR_RED, COLOR_GREEN, 100,30, tmp);
    }
    else
    {
        sprintf(tmp,"%d", width);
        gfx_putS(COLOR_BLACK, COLOR_GREEN, 5,30, "Width");
        gfx_putS(COLOR_BLACK, COLOR_GREEN, 100,30, tmp);
    }

    if(settingParam == 1)
    {
        sprintf(tmp,"%d", height);
        gfx_putS(COLOR_RED, COLOR_GREEN, 5,50, "Height");
        gfx_putS(COLOR_RED, COLOR_GREEN, 100,50, tmp);
    }
    else
    {
        sprintf(tmp,"%d", height);
        gfx_putS(COLOR_BLACK, COLOR_GREEN, 5,50, "Height");
        gfx_putS(COLOR_BLACK, COLOR_GREEN, 100,50, tmp);
    }

    if(settingParam == 2)
    {
        sprintf(tmp,"%d", p);
           gfx_putS(COLOR_RED, COLOR_GREEN, 5,70, "Mines[%]");
        gfx_putS(COLOR_RED, COLOR_GREEN, 100,70, tmp);
    }
    else
    {
        sprintf(tmp,"%d", p);
        gfx_putS(COLOR_BLACK, COLOR_GREEN, 5,70, "Mines[%]");
        gfx_putS(COLOR_BLACK, COLOR_GREEN, 100,70, tmp);
    }

    g_changes = 1;
}

void init_settings_screen()
{
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 5,5, "S E T T I N G S");

    RefreshSettings();
}

/* discovers the tile when player clears one of them */
/* a chain reaction (of discovery) occurs if tile has no mines */
/* as neighbors */
void discover(int, int);
void discover(int x, int y){

    if(x<0) return;
    if(y<0) return;
    if(x>width-1) return;
    if(y>height-1) return;
    if(MINE_FIELD(y,x).known) return;

    MINE_FIELD(y,x).known = 1;
    if(MINE_FIELD(y,x).neighbors == 0){
        discover(x-1,y-1);
        discover(x,y-1);
        discover(x+1,y-1);
        discover(x+1,y);
        discover(x+1,y+1);
        discover(x,y+1);
        discover(x-1,y+1);
        discover(x-1,y);
    }
    return;
}

/* init not mine related elements of the mine field */
void minesweeper_init(void){
    int i,j;

    readMinesINI();

    tiles_left=width*height;
    mine_num = 0;

    //time = GetTime();

    srand(tmr_getTick());

    gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,17, "New game");
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 295,47, "Quit");
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,152, "Discover");
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,180, "Set Flag");
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,210, "Settings");

    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            MINE_FIELD(i,j).known = 0;
            MINE_FIELD(i,j).flag = 0;
        }
    }
}

void printNumberOfMines(void)
{
    char tmp[20];
     int i=0;
     int j=0;

     if(mine_num > 0)
     {
        tiles_left = 0;
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                    if(MINE_FIELD(i,j).flag) tiles_left++;
            }
        }

        gfx_fillRect(COLOR_GREEN, 265,79, 54, 15);
        sprintf(tmp,"%d Mines", mine_num-tiles_left);
        gfx_putS(COLOR_WHITE, COLOR_GREEN, 266,80, tmp);
     }
}

/* put mines on the mine field */
/* there is p% chance that a tile is a mine */
/* if the tile has coordinates (x,y), then it can't be a mine */
void minesweeper_putmines(int p, int x, int y){
    int i,j;
/*
     char tmp[10];
        sprintf(tmp,"p: %ld", p);
        gfx_putS(COLOR_WHITE, COLOR_GREEN, 266,110, tmp);
*/
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            if(rand()%100<p && !(y==i && x==j)){
                MINE_FIELD(i,j).mine = 1;
                mine_num++;
            } else {
                MINE_FIELD(i,j).mine = 0;
            }
            MINE_FIELD(i,j).neighbors = 0;
        }
    }

    /* we need to compute the neighbor element for each tile */
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            if(i>0){
                if(j>0)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i-1,j-1).mine;
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i-1,j).mine;
                if(j<width-1)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i-1,j+1).mine;
            }
            if(j>0)
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i,j-1).mine;
            if(j<width-1)
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i,j+1).mine;
            if(i<height-1){
                if(j>0)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i+1,j-1).mine;
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i+1,j).mine;
                if(j<width-1)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i+1,j+1).mine;
            }
        }
    }
}

void setCursor(int del)
{
   if(del)
       gfx_drawRect(COLOR_GREEN, x*PIECE_DIM, y*PIECE_DIM, PIECE_DIM, PIECE_DIM);
    else
       gfx_drawRect(COLOR_RED, x*PIECE_DIM, y*PIECE_DIM, PIECE_DIM, PIECE_DIM);
}

void eventHandlerLoop(void)
{
    int i=0,j=0;
	int stop=0;
	int evt;
	int evt_handler=evt_getHandler(BTN_CLASS);
    while(!stop)
    {
		evt=evt_getStatus(evt_handler);
		if(evt==NO_EVENT)
			continue;

		if(GameMode == 0)
		{
			switch (evt) {
				case BTN_OFF:
						/* get out of here */
						stop=1;
						break;
	
						/* move cursor left */
					case BTN_LEFT:
						setCursor(1);
				   if(x-1 >= 0)
						   x--;
						else
						   x = width-1;
	
						setCursor(0);
						break;
	
						/* move cursor right */
					case BTN_RIGHT:
						setCursor(1);
				   if(x+1 >= width)
						   x = 0;
						else
						   x++;
						setCursor(0);
						break;
	
						/* move cursor down */
					case BTN_DOWN:
						setCursor(1);
				   if(y+1 >= height)
						   y = 0;
						else
						   y++;
						setCursor(0);
						break;
	
						/* move cursor up */
					case BTN_UP:
						setCursor(1);
				   if(y-1 >= 0)
						   y--;
						else
						   y = height-1;
						setCursor(0);
						break;
	
						/* discover a tile (and it's neighbors if .neighbors == 0) */
					case BTN_F1:
						if(MINE_FIELD(y,x).flag) break;
						/* we put the mines on the first "click" so that you don't */
						/* lose on the first "click" */
						if(tiles_left == width*height) minesweeper_putmines(p,x,y);
						discover(x,y);
						if(MINE_FIELD(y,x).mine)
						{
							GameMode = 1;
							x=0;y=0;
							gfx_clearScreen(COLOR_GREEN);
							gfx_putS(COLOR_BLACK, COLOR_GREEN, 120, 100, "You lose!");
							gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,17, "New game");
							gfx_putS(COLOR_BLACK, COLOR_GREEN, 295,47, "Quit");
#warning we had return 1 here
							break;
						}
						tiles_left = 0;
						for(i=0;i<height;i++){
							for(j=0;j<width;j++){
									if(MINE_FIELD(i,j).known == 0) tiles_left++;
							}
						}
						if(tiles_left == mine_num)
						{
							GameMode = 1;
							x=0;y=0;
							gfx_clearScreen(COLOR_GREEN);
							gfx_putS(COLOR_BLACK, COLOR_GREEN, 120, 100, "You win!");
							gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,17, "New game");
							gfx_putS(COLOR_BLACK, COLOR_GREEN, 295,47, "Quit");
#warning we had return 1 here
							break;
						}
	
						displayMineField();
						setCursor(0);
						break;
	
						/* toggle flag under cursor */
					case BTN_F2:
						MINE_FIELD(y,x).flag = (MINE_FIELD(y,x).flag + 1)%2;
						displayMineField();
						break;
	
					case BTN_ON: // new game
						gfx_clearScreen(COLOR_GREEN);
						minesweeper_init();
						displayMineField();
						setCursor(0);
						break;
	
					case BTN_F3: // settings
						gfx_clearScreen(COLOR_GREEN);
						gfx_putS(COLOR_BLACK, COLOR_GREEN, 295,47, "Quit");
						GameMode = 2;
						init_settings_screen();
						break;
			}
		}
		else if(GameMode == 1)
		{
			switch (evt) {
				case BTN_OFF:
						/* get out of here */
						stop=1;
						break;
	
				case BTN_ON: // new game
					gfx_clearScreen(COLOR_GREEN);
					GameMode = 0;
					x=0;y=0;
					minesweeper_init();
					displayMineField();
					setCursor(0);
					break;
			}
		}
		else
		{
			switch (evt) {
			   case BTN_UP:
					if(settingParam == 0)
					   settingParam = 2;
					else
					   settingParam--;
	
					RefreshSettings();
				   break;
	
			   case BTN_DOWN:
					if(settingParam == 2)
					   settingParam = 0;
					else
					   settingParam++;
	
					RefreshSettings();
				   break;
	
			   case BTN_LEFT:
	
					if(settingParam == 0)
					{
					   if(width > 10)
						{
					  width--;
					  RefreshSettings();
						}
					}
					else if(settingParam == 1)
					{
					   if(height > 10)
						{
					  height--;
					  RefreshSettings();
						}
					}
					else
					{
					   if(p > 0)
						{
						   p--;
					  RefreshSettings();
						}
					}
				   break;
	
			   case BTN_RIGHT:
				   if(settingParam == 0)
					{
					   if(width < (screen_width-50)/PIECE_DIM)
						{
					  width++;
					  RefreshSettings();
						}
					}
					else if(settingParam == 1)
					{
					   if(height < (screen_height/PIECE_DIM))
						{
					  height++;
					  RefreshSettings();
				   }
					}
					else
					{
					   if(p < 100)
						{
						   p++;
					  RefreshSettings();
						}
					}
				   break;
	
				case BTN_OFF:
					writeMinesINI();
					gfx_clearScreen(COLOR_GREEN);
					GameMode = 0;
					x=0;y=0;
					minesweeper_init();
					displayMineField();
					setCursor(0);
					break;
			}
	
		}
	}
	evt_freeHandler(evt_handler);
}

void displayMineField()
{
   int i=0,j=0;

    //display the mine field
    for(i=0;i<height;i++)
    {
            for(j=0;j<width;j++)
            {
                if(MINE_FIELD(i,j).known)
                {
                    if(MINE_FIELD(i,j).mine)
                    {
                      gfx_putS(COLOR_RED, COLOR_LIGHT_GREY, 5j*PIECE_DIM+1,i*PIECE_DIM+1,"b");
                    }
                    else if(MINE_FIELD(i,j).neighbors)
                    {
                            if(MINE_FIELD(i,j).neighbors == 0)
                                gfx_drawBitmap (&emptyB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 1)
                                gfx_drawBitmap (&oneB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 2)
                                gfx_drawBitmap (&twoB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 3)
                                gfx_drawBitmap (&threeB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 4)
                                gfx_drawBitmap (&fourB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 5)
                                gfx_drawBitmap (&fiveB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 6)
                                gfx_drawBitmap (&sixB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 7)
                                gfx_drawBitmap (&sevenB, j*PIECE_DIM,i*PIECE_DIM);
                            else if(MINE_FIELD(i,j).neighbors == 8)
                                gfx_drawBitmap (&eightB, j*PIECE_DIM,i*PIECE_DIM);
                    }
                    else
                    {
                       gfx_fillRect(COLOR_GREY, j*PIECE_DIM+1,i*PIECE_DIM+1,PIECE_DIM-2,PIECE_DIM-2);
                    }
                }
                else if(MINE_FIELD(i,j).flag)
                {
                    gfx_fillRect(COLOR_LIGHT_GREY, j*PIECE_DIM+1,i*PIECE_DIM+1,PIECE_DIM-2,PIECE_DIM-2);
                    gfx_drawLine(COLOR_RED, j*PIECE_DIM+2,i*PIECE_DIM+2,j*PIECE_DIM+PIECE_DIM-2,i*PIECE_DIM+PIECE_DIM-2);
                    gfx_drawLine(COLOR_RED, j*PIECE_DIM+2,i*PIECE_DIM+PIECE_DIM-2,j*PIECE_DIM+PIECE_DIM-2,i*PIECE_DIM+2);
                }
                else
                {
                    gfx_fillRect(COLOR_LIGHT_GREY, j*PIECE_DIM+1,i*PIECE_DIM+1,PIECE_DIM-2,PIECE_DIM-2);
                }
            }
    }

   printNumberOfMines();
}

/* plugin entry point */
void app_main(int argc,char * * argv)
{
    gfx_clearScreen(COLOR_GREEN);
    /* end of plugin init */

    /* use standard font */
    gfx_fontSet(STD6X9);

	getResolution(&screen_width,&screen_height);
	height = screen_height/PIECE_DIM;
	width = (screen_width-50)/PIECE_DIM;
	
	minefield = (tile*)malloc(sizeof(tile)*height*width);
    
	minesweeper_init();
    displayMineField();
    setCursor(0);

	eventHandlerLoop();
}

