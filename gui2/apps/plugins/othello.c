/*
* othello.c
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

#include <stdio.h>

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "graphics.h"
#include "events.h"
#include "colordef.h"
#include "font.h"

#include "cops.h"
#include "avevents.h"

struct client_operations * cops;
struct position {
    int x;
    int y;
};

struct position cursor_pos={3,2};

#define CELL_SIZE  20
#define PIECE_SIZE 8
#define NB_CELL    8
#define X_INI      10
#define Y_INI      ((240-((CELL_SIZE+1)*NB_CELL)-1)/2)



#define NB_PIECE_POS 90

#define BG_COLOR     COLOR_BLACK
#define LINE_COLOR   COLOR_WHITE
#define SEL_COLOR    COLOR_RED
#define PIECE_COLOR  COLOR_BLUE
#define TXT_COLOR    COLOR_BLUE

#define EMPTY      -1
#define HUMAN      0
#define AV3XX      1

int board[NB_CELL][NB_CELL];
int nbPieces[2]={2,2};

unsigned char human_bmap[8][8]= {
{BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR},
{PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR},
{PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR},
{BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR}
};

unsigned char av3xx_bmap[8][8]= {
{PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR},
{BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR},
{BG_COLOR,PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR,BG_COLOR},
{PIECE_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,BG_COLOR,PIECE_COLOR}
};

BITMAP hBitmap = {(unsigned int) human_bmap, 8, 8, 0, 0};
BITMAP aBitmap = {(unsigned int) av3xx_bmap, 8, 8, 0, 0};

#define PIECE_OFFSET (CELL_SIZE-PIECE_SIZE)/2+1

void drawPiece(int x, int y, int player)
{
    switch(player)
    {
        case HUMAN:
            cops->drawBITMAP(&hBitmap,X_INI+PIECE_OFFSET+x*(CELL_SIZE+1),Y_INI+PIECE_OFFSET+y*(CELL_SIZE+1));
            break;
        case AV3XX:
            cops->drawBITMAP(&aBitmap,X_INI+PIECE_OFFSET+x*(CELL_SIZE+1),Y_INI+PIECE_OFFSET+y*(CELL_SIZE+1));
            break;
    }
}

void erasePiece(int x, int y)
{
    cops->fillRect(BG_COLOR,X_INI+1+x*(CELL_SIZE+1),Y_INI+1+x*(CELL_SIZE+1),CELL_SIZE,CELL_SIZE);
}

void drawBoard(void)
{
    int i,j;
    
    cops->clearScreen(BG_COLOR);
    
    
    for(i=0;i<NB_CELL+1;i++)
    {
        cops->drawLine(LINE_COLOR,X_INI,Y_INI+i*(CELL_SIZE+1),X_INI+NB_CELL*(CELL_SIZE+1),Y_INI+i*(CELL_SIZE+1)); /* horizontal line */
        cops->drawLine(LINE_COLOR,X_INI+i*(CELL_SIZE+1),Y_INI,X_INI+i*(CELL_SIZE+1),Y_INI+NB_CELL*(CELL_SIZE+1)); /* vertical line */
    }
    
    for(i=0;i<NB_CELL;i++)
        for(j=0;j<NB_CELL;j++)
            if(board[i][j]!=EMPTY)
                drawPiece(i, j,board[i][j]);
}

void selectCell(int x, int y)
{
    cops->drawRect(SEL_COLOR,X_INI+x*(CELL_SIZE+1),Y_INI+y*(CELL_SIZE+1),CELL_SIZE+2,CELL_SIZE+2);
}

void unSelectCell(int x, int y)
{
    cops->drawRect(LINE_COLOR,X_INI+x*(CELL_SIZE+1),Y_INI+y*(CELL_SIZE+1),CELL_SIZE+2,CELL_SIZE+2);
}

void drawNbPiece()
{
    char tmp[10];
    int w=0,h=0;
    
    sprintf(tmp,"You: %02d",nbPieces[HUMAN]);
    cops->getStringS(tmp, &w, &h);
    cops->putS(TXT_COLOR,BG_COLOR,320-w-15,NB_PIECE_POS,tmp);
    sprintf(tmp,"AV3XX: %02d",nbPieces[AV3XX]);
    cops->getStringS(tmp, &w, &h);
    cops->putS(TXT_COLOR,BG_COLOR,320-w-15,NB_PIECE_POS+h+2,tmp);
}

void drawMenu()
{
    int w=0,h=0;
    
    cops->getStringS("New game", &w, &h);
    cops->putS(TXT_COLOR, BG_COLOR, 320-w-5,17, "New game");
    
    cops->getStringS("Quit", &w, &h);
    cops->putS(TXT_COLOR, BG_COLOR, 320-w-5,47, "Quit");
    
    cops->getStringS("Play", &w, &h);
    cops->putS(TXT_COLOR, BG_COLOR, 320-w-5,152, "Play");
    
    /*cops->getStringS("Set Flag", &w, &h);
    cops->putS(TXT_COLOR, BG_COLOR, 320-w-5,180, "Set Flag");*/
    
    cops->getStringS("Settings", &w, &h);
    cops->putS(TXT_COLOR, BG_COLOR, 320-w-5,210, "Settings");
}

void iniBoard(void)
{
    int i,j;

    for(i=0;i<NB_CELL;i++)
        for(j=0;j<NB_CELL;j++)
        {
            board[i][j]=EMPTY;
        }
    i=NB_CELL/2-1;
    
    /*setting initial piece*/
    board[i][i]=HUMAN;board[i+1][i]=AV3XX;
    board[i][i+1]=AV3XX;board[i+1][i+1]=HUMAN;    
}

void moveCursor(int dx, int dy)
{
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x+=dx;
    cursor_pos.y+=dy;
    selectCell(cursor_pos.x,cursor_pos.y);
}

int eventHandler(int evt)
{
    int offset;
    switch(evt)
    {
        case BTN_UP:
            offset=-1;
            while((cursor_pos.y+offset)>=0 && board[cursor_pos.x][cursor_pos.y+offset]!=EMPTY)
                offset--;
            if((cursor_pos.y+offset)>=0)
                moveCursor(0,offset);            
            break;
        case BTN_DOWN:
            offset=1;
            while((cursor_pos.y+offset)<=(NB_CELL-1) && board[cursor_pos.x][cursor_pos.y+offset]!=EMPTY)
                offset++;
            if((cursor_pos.y+offset)<=(NB_CELL-1))
                moveCursor(0,offset);
            break;
        case BTN_LEFT:
            offset=-1;
            while((cursor_pos.x+offset)>=0 && board[cursor_pos.x+offset][cursor_pos.y]!=EMPTY)
                offset--;
            if((cursor_pos.x+offset)>=0)
                moveCursor(offset,0);            
            break;
        case BTN_RIGHT:
            offset=1;
            while((cursor_pos.x+offset)<=(NB_CELL-1) && board[cursor_pos.x+offset][cursor_pos.y]!=EMPTY)
                offset++;
            if((cursor_pos.x+offset)<=(NB_CELL-1))
                moveCursor(offset,0);
            break;
        case BTN_OFF:
        case EVT_QUIT:
            RELEASE(cops)
            break;
        case EVT_REDRAW:
            drawBoard();
            drawMenu();
            selectCell(cursor_pos.x,cursor_pos.y);
            drawNbPiece();
            break;
    }
    return 1;
}

int main(int argc,char ** argv)
{
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();  
    cops->disableMenu();
    iniBoard();
    eventHandler(EVT_REDRAW);
    PACK(cops,NULL);
    STOPME(cops);
    return 1;
}
