/*
* empire.c
* by Schoki
*
* linav - http://linav.sourceforge.net
* Copyright (c) 24 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

/////////////////////////////////////////////////////////////////////
// A strategy game to explore a world and knock out your opponent.
// Based on original amiga empire game.
/////////////////////////////////////////////////////////////////////

/* ToDos:
- remember all fields which are already visible for each player(auch evtl Objekte die darauf plaziert sind)
- define bitmaps of all objects (maybe sprites for different colors)
- Manage list of objects for each player (xPos,yPos,Already-moved-flag)
- Property list for each available object in the game (Strange,access radius,name, move flag for the different world elements)
- End-Turn-Button
*/
/////////////////////////////////////////////////////////////////////

#include "stdlib.h"

#include "colordef.h"
#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"

#define true 1
#define false 0

#define PIECE_DIM 15

#define WORLD_X 15
#define WORLD_Y 15

#define VERSION "Empire V0.002"

struct client_operations * cops;

struct object_properties
{
	char name[15];
   int strange;        // ( 0 - 20)
	int defense;        // ( 0 - 20)
	int access_radius;  // ( 0 - 3)
	int hitpoints;      // ( 0 - 5), decreased by every hit by one
	int buildturns;     // ( 0 - 20)

	// MoveBits
	// Bit 0 -> cant move
	// Bit 1 -> can move on forests
	// Bit 2 -> can move on hills
	// Bit 4 -> can move on water
	int moveBits;
};

static struct object_properties Objects[] =
{
// Name              Str  Def Rad Hit Bld  Move(Bits)
   {"Town",            0,  10,  0,  5,  0, 0x00},
   {"Armee",           5,   5,  1,  1,  3,  3},
   {"Tank",           10,   8,  1,  3,  8, 0x01},
   {"Plane",          10,   3,  3,  2,  8, 0x07},
   {"Destroyer",      15,   8,  2,  3, 12, 0x04},
   {"Battleship",     20,  12,  2,  5, 15, 0x04},
   {"Transportship",   5,   1,  2,  2, 10, 0x04}
};

int startposX = 1;
int startposY = 1;

static unsigned char forest[PIECE_DIM][PIECE_DIM] = { // 0
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,80,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,80,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {80,80,80,10,10,10,10,10,10,10,10,10,10,10,10},
  {80,80,80,10,10,10,10,10,10,10,10,10,10,10,10},
  {80,80,80,10,10,10,10,10,10,80,10,10,10,10,10},
  {10,98,10,10,10,10,10,10,10,80,10,10,10,10,10},
  {10,98,10,10,10,10,10,10,80,80,80,10,10,10,10},
  {10,98,10,10,10,10,10,10,80,80,80,10,10,10,10},
  {10,10,10,10,10,10,10,10,80,80,80,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,98,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,98,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,98,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10} };

static unsigned char town[PIECE_DIM][PIECE_DIM] = {  // 1
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10, 7,10, 7,10,10,10,10,10},
  {10,10,10,10,10,10,10, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10,10,10,10, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10,10, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10, 7, 7, 7, 7, 7, 7,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10} };

static unsigned char water[PIECE_DIM][PIECE_DIM] = { //10
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9} };

static unsigned char hill[PIECE_DIM][PIECE_DIM] = { // 3
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,15,15,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,15,15,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,98,98,98,98,10,10,10,10,10},
  {10,10,10,10,10,98,98,98,98,98,10,10,10,10,10},
  {10,10,10,10,10,98,98,98,98,98,10,10,10,10,10},
  {10,10,10,10,98,98,98,98,98,98,10,10,10,10,10},
  {10,10,10,98,98,98,98,98,98,98,98,10,10,10,10},
  {10,10,10,98,98,98,98,98,98,98,98,10,10,10,10},
  {10,10,98,98,98,98,98,98,98,98,98,98,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10} };

static unsigned char tank[PIECE_DIM][PIECE_DIM] = {
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10, 8, 8,10,10,10,10,10,10,10},
  {10,10, 8, 8, 8, 8, 8, 8, 8,10,10,10,10,10,10},
  {10,10,10,10,10,10, 8, 8, 8,10,10,10,10,10,10},
  {10,10,10,10,10,10, 8, 8, 8, 8,10,10,10,10,10},
  {10,10,10, 8, 8, 8, 8, 8, 8, 8, 8, 8,10,10,10},
  {10,10,10, 8, 8, 8, 8, 8, 8, 8, 8, 8,10,10,10},
  {10,10,10, 8, 8, 8, 8, 8, 8, 8, 8, 8,10,10,10},
  {10,10,10, 8, 8, 8, 8, 8, 8, 8, 8, 8,10,10,10},
  {10,10,10, 8, 8, 8, 8, 8, 8, 8, 8,10,10,10,10},
  {10,10,10,10,10, 8, 8, 8, 8, 8,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10} };

static unsigned char worldmap[WORLD_X][WORLD_Y] = {
  {0,0,0,0,0,0,3,3,3,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,3,3,0,0,0,0,0,0},
  {0,0,1,0,0,0,0,0,0,2,2,2,2,2,0},
  {2,2,0,0,0,0,0,0,2,0,0,0,0,1,0},
  {2,2,0,0,0,0,0,2,0,0,0,0,0,0,0},
  {2,2,0,0,0,0,2,0,0,0,0,0,0,0,0},
  {2,2,0,0,0,2,0,0,0,0,0,0,0,0,0},
  {2,2,2,2,2,0,0,0,0,0,0,0,0,0,0},
  {0,2,2,2,0,0,0,0,3,3,0,0,0,0,0},
  {0,2,2,2,2,0,0,3,3,3,0,0,0,0,0},
  {2,2,2,2,2,0,0,3,0,0,0,0,0,0,0},
  {2,2,2,2,2,0,3,3,0,0,0,0,1,0,0},
  {2,2,2,2,2,0,0,0,0,0,0,0,0,0,0},
  {2,1,2,2,2,1,0,0,0,0,0,0,3,3,3},
  {2,2,2,2,2,2,0,0,0,0,0,0,3,3,3} };

static BITMAP forestB = {(unsigned int) forest, PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP townB   = {(unsigned int) town,   PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP waterB  = {(unsigned int) water,  PIECE_DIM, PIECE_DIM, 0, 0};
static BITMAP hillB   = {(unsigned int) hill,   PIECE_DIM, PIECE_DIM, 0, 0};

static BITMAP tankB   = {(unsigned int) tank,   PIECE_DIM, PIECE_DIM, 0, 0};

/*********************************************************
/
/ border_check
/
*********************************************************/
static int border_check(int evt, int x, int y, int offset)
{
	if(evt == BTN_RIGHT)
	{
      if(x+offset >= WORLD_X) return 0;
   }
	else if( evt == BTN_LEFT)
	{
      if(x-offset < 0)
		{
		   return 0;
		}
	}
	else if( evt == BTN_DOWN)
	{
      if(y+offset >= WORLD_Y) return 0;
	}
	else if( evt == BTN_UP)
	{
      if(y-offset < 0) return 0;
   }

	return 1;
}

/*********************************************************
/
/ drawDifferentPieces
/
*********************************************************/
static void drawDifferentPieces(int x, int y)
{
	if(worldmap[y][x] ==  0)
		cops->drawBITMAP (&forestB, x*PIECE_DIM, y*PIECE_DIM);
	if(worldmap[y][x] ==  1)
		cops->drawBITMAP (&townB,   x*PIECE_DIM, y*PIECE_DIM);
	if(worldmap[y][x] ==  2)
		cops->drawBITMAP (&waterB,  x*PIECE_DIM, y*PIECE_DIM);
	if(worldmap[y][x] ==  3)
		cops->drawBITMAP (&hillB,   x*PIECE_DIM, y*PIECE_DIM);
}

/*********************************************************
/
/ draw_piece
/
*********************************************************/
static void draw_piece(int x, int y, int withRadius)
{
   int i = 0;

	drawDifferentPieces(x, y); // Draw piece on current position

	// draw pieces around the current position if needed
	if(withRadius > 0)
	{
      for(i=1;i<=withRadius;i++)
		{
		   if(border_check(BTN_LEFT,x-i,y,0))
      	   drawDifferentPieces(x-i, y);

		   if(border_check(BTN_RIGHT,x+i,y,0))
         	drawDifferentPieces(x+i, y);

		   if(border_check(BTN_UP,x,y-i,0))
         	drawDifferentPieces(x, y-i);

		   if(border_check(BTN_DOWN,x,y+i,0))
         	drawDifferentPieces(x, y+i);

		   if(border_check(BTN_LEFT,x-i,y,0) && border_check(BTN_UP,x,y-i,0))
         	drawDifferentPieces(x-i, y-i);

		   if(border_check(BTN_RIGHT,x+i,y,0) && border_check(BTN_DOWN,x,y+i,0))
         	drawDifferentPieces(x+i, y+i);

		   if(border_check(BTN_RIGHT,x+i,y,0) && border_check(BTN_UP,x,y-i,0))
         	drawDifferentPieces(x+i, y-i);

		   if(border_check(BTN_LEFT,x-i,y,0) && border_check(BTN_DOWN,x,y+i,0))
         	drawDifferentPieces(x-i, y+i);
		}
	}
}

/*********************************************************
/
/ draw_world
/
*********************************************************/
static void draw_world()
{
   int x = 0;
	int y = 0;

	// ToDo: Scrollbereich berücksichtigen
	for( x=0; x<WORLD_X; x++)
	{
   	for( y=0; y<WORLD_Y; y++)
		{
		   draw_piece(x,y,0);
		}
	}
}

/*********************************************************
/
/ empire_init
/
*********************************************************/
static void empire_init(void)
{
    cops->clearScreen(COLOR_WHITE);

	 // ToDo: Draw Map out of a file or just generated by prog
//    draw_world();
//    draw_piece(startposX, startposY, 1);
  	 cops->drawBITMAP (&tankB, startposX*PIECE_DIM, startposY*PIECE_DIM);

    // Startposition einzeichnen
	 cops->drawRect(COLOR_RED, startposX*PIECE_DIM, startposY*PIECE_DIM, PIECE_DIM, PIECE_DIM);

    cops->putS(COLOR_BLACK, COLOR_WHITE, 230,  5, VERSION);
    cops->putS(COLOR_BLACK, COLOR_WHITE, 239, 20, "by Schoki");
}

/*********************************************************
/
/ move_position
/
*********************************************************/
static void move_position(int evt)
{
   char tmp[10];
   int ret = 0;

	ret = border_check(evt,startposX,startposY,1);
	if(ret == 0)
	   return;

   // roten Rahmen an der alten Position entfernen
   draw_piece(startposX,startposY,0);

	// Neue Position markieren
	if(evt == BTN_RIGHT)
      startposX++;
	else if( evt == BTN_LEFT)
      startposX--;
	else if( evt == BTN_DOWN)
      startposY++;
	else if( evt == BTN_UP)
      startposY--;

	cops->fillRect(COLOR_WHITE,228,55,80,25);
	sprintf(tmp,"x:%d y:%d", startposX, startposY);
   cops->putS(COLOR_BLACK, COLOR_WHITE, 230, 60, tmp);

   // neues Stück zeichnen
   draw_piece(startposX,startposY,1);

	cops->drawRect(COLOR_RED, startposX*PIECE_DIM, startposY*PIECE_DIM, PIECE_DIM, PIECE_DIM);
}

/*********************************************************
/
/ eventHandler
/
*********************************************************/
int eventHandler(int evt)
{
   int i;

	switch (evt) {
			case BTN_OFF:
			case EVT_QUIT:
				RELEASE(cops)
				break;

			case BTN_F1:
				break;

			case BTN_F2:
				break;

			case BTN_LEFT:
            move_position(evt);
				break;

			case BTN_RIGHT:
            move_position(evt);
				break;

			case BTN_UP:
            move_position(evt);
				break;

			case BTN_DOWN:
            move_position(evt);
				break;
	}
}

/*********************************************************
/
/ main
/
*********************************************************/
int main(int argc,char * * argv)
{
	 REGISTER(cops,eventHandler,0);

    cops->hideSBar();

    empire_init();

    PACK(cops,NULL);
    STOPME(cops)

    cops->showSBar();

    return;
}

