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

#define JUST_TEST   0
#define NO_MOVE     0
#define CHK_ALL     1
#define DO_MOVE     1

#define NXT_POS     0
#define PREV_POS    1

#define SEARCH_DEPTH 4

#define DEBUG

int board[NB_CELL][NB_CELL];
int allowedHuman[NB_CELL][NB_CELL];
int nbPieces[2]={2,2};

int endOfGame=0;

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
    cops->fillRect(BG_COLOR,X_INI+1+x*(CELL_SIZE+1),Y_INI+1+y*(CELL_SIZE+1),CELL_SIZE,CELL_SIZE);
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
    
    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
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
    
#ifdef DEBUG
    printf("NB:(%d,%d)\n",nbPieces[HUMAN],nbPieces[AV3XX]);
#endif
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

    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
        {
            board[i][j]=EMPTY;
        }
    i=NB_CELL/2-1;
    
    /*setting initial piece*/
    board[i][i]=AV3XX;board[i+1][i]=HUMAN;
    board[i][i+1]=HUMAN;board[i+1][i+1]=AV3XX;    
}

void moveCursor(int dx, int dy)
{
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x+=dx;
    cursor_pos.y+=dy;
    selectCell(cursor_pos.x,cursor_pos.y);
}

int tstMove(int player,int x, int y,int dx, int dy,int dispMove)
{
    int other=(player==HUMAN?AV3XX:HUMAN);
    int out_of_board=0;
    int tot=0,i;
    x+=dx;y+=dy;
   // printf("[tstMove] dx=%d dy=%d\n",dx,dy);
    while(board[x][y]==other)
    {
        x+=dx;y+=dy;
        tot+=1;
        if(x<0 || x>=NB_CELL || y<0 || y>=NB_CELL)
        {
            out_of_board=1;
            break;
        }
    }
    
    if(!out_of_board && board[x][y]==player)
    {
        if(dispMove)
        {
            printf("display, get %d, dx=%d dy=%d\n",tot,dx,dy);
            for(i=0;i<tot;i++)
            {
                x-=dx;y-=dy;
                erasePiece(x,y);
                drawPiece(x,y,player);
                board[x][y]=player;
            }
            printf("\n");
            
        }
        return tot;
    }
    
    return 0;
    
}

int validMove(int player,int x,int y,int chkAll,int dispMove)
{
    int other=(player==HUMAN?AV3XX:HUMAN);
    
    int tot=0;
    
    if(x>=2 && board[x-1][y]==other && (tot+=tstMove(player,x,y,-1,0,dispMove)))
        if(!chkAll)
            return 1;
    if(x<=(NB_CELL-3) && board[x+1][y]==other && (tot+=tstMove(player,x,y,1,0,dispMove)))
        if(!chkAll)
            return 1;
    if(y>=2 && board[x][y-1]==other && (tot+=tstMove(player,x,y,0,-1,dispMove)))
        if(!chkAll)
            return 1;
    if(y<=(NB_CELL-3) && board[x][y+1]==other && (tot+=tstMove(player,x,y,0,1,dispMove)))
        if(!chkAll)
            return 1;
    
    if(x>=2 && y>=2 && board[x-1][y-1]==other && (tot+=tstMove(player,x,y,-1,-1,dispMove)))
        if(!chkAll)
            return 1;
    if(x<=(NB_CELL-3) && y>=2 && board[x+1][y-1]==other && (tot+=tstMove(player,x,y,1,-1,dispMove)))
        if(!chkAll)
            return 1;
    if(x>=2 && y<=(NB_CELL-3) && board[x-1][y+1]==other && (tot+=tstMove(player,x,y,-1,1,dispMove)))
        if(!chkAll)
            return 1;
    if(x<=(NB_CELL-3) && y<=(NB_CELL-3) && board[x+1][y+1]==other && (tot+=tstMove(player,x,y,1,1,dispMove)))
        if(!chkAll)
            return 1;
        
    return tot;
}

int computeAllowed(int allowed[NB_CELL][NB_CELL],int player)
{
    int i,j,tot=0;
    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
        {
            if(board[i][j] == EMPTY && validMove(player,i,j,JUST_TEST,JUST_TEST))
            {
                tot++;
                allowed[i][j]=1;
            }
            else
            {
                allowed[i][j]=0;
            }
        }
    return tot;
}

int computerMove(void)
{
    int i,j,tot,x=0,y=0,max=0;
    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
        {
            if(board[i][j] == EMPTY && (tot=validMove(AV3XX,i,j,JUST_TEST,JUST_TEST)))
            {
                if(tot>max)
                {
                    max=tot;
                    x=i;
                    y=j;
                }  
            }            
        }
     if(max>0)
     {
         tot=validMove(AV3XX,x,y,CHK_ALL,DO_MOVE);
         board[x][y]=AV3XX;
         drawPiece(x,y,AV3XX);
         nbPieces[AV3XX]+=tot;
         nbPieces[AV3XX]++;
         nbPieces[HUMAN]-=tot;
         return 1;
     }
     else
         return 0;
     
}

void doMove(int x,int y)
{
    int tot;
    tot=validMove(HUMAN,x,y,CHK_ALL,DO_MOVE);
    board[x][y]=HUMAN;
    drawPiece(x,y,HUMAN); 
    nbPieces[HUMAN]+=tot;
    nbPieces[HUMAN]++;
    nbPieces[AV3XX]-=tot;   
}

void endGame(void)
{
    int w,h,h2;
    
    cops->getStringS("M",&w,&h);
    
    w=100;
    h2=(h+2)*2+8;
        
    cops->fillRect(COLOR_BLACK,(SCREEN_WIDTH-w)/2,(SCREEN_HEIGHT-h2)/2,w,h2);
    cops->putS(COLOR_WHITE,COLOR_BLACK,(SCREEN_WIDTH-w)/2+5,(SCREEN_HEIGHT-h2)/2+5,"End of game");
    if(nbPieces[HUMAN]>nbPieces[AV3XX])
         cops->putS(COLOR_WHITE,COLOR_BLACK,(SCREEN_WIDTH-w)/2+5,(SCREEN_HEIGHT-h2)/2+5+2+h,"You win");
    else if(nbPieces[HUMAN]<nbPieces[AV3XX])
        cops->putS(COLOR_WHITE,COLOR_BLACK,(SCREEN_WIDTH-w)/2+5,(SCREEN_HEIGHT-h2)/2+5+2+h,"You loose");
    else
        cops->putS(COLOR_WHITE,COLOR_BLACK,(SCREEN_WIDTH-w)/2+5,(SCREEN_HEIGHT-h2)/2+5+2+h,"Tie");
            
    
}

#ifdef DEBUG
void printBoardAllowed(void)
{
    int i,j;
    
    for(j=0;j<NB_CELL;j++)
    {
        for(i=0;i<NB_CELL;i++)
            printf("--");
        printf("-\n");
        for(i=0;i<NB_CELL;i++)
            if(board[i][j]==HUMAN)
                printf("|o");
            else if(board[i][j]==AV3XX)
                printf("|x");
            else if(allowedHuman[i][j])
                printf("|a");
            else if(board[i][j]==EMPTY)
                printf("| ");
            printf("|\n");
    }
    for(i=0;i<NB_CELL;i++)
        printf("--");
    printf("-\n");
}
#endif

void nxtCursosPos(int direction,int dispMove)
{
    int i,j;
    int stop;        
    
    if(direction == PREV_POS)
    {
        printf("Find prev pos\n");
        stop=0;
        j=cursor_pos.y;
        i=cursor_pos.x-1;
        while(1)
        {
            while(j>=0 && !stop)
            {
                while(i>=0 && !stop)
                {
                    if(allowedHuman[i][j]==1)
                    {
                        stop=1;
                        break;
                    }
                    i--;
                }
                if(stop)
                    break;
                i=NB_CELL-1;
                j--;
            }
            if(stop)
            {
                if(dispMove==DO_MOVE)
                {
                    printf("doing Move (%d,%d)->(%d,%d)\n",cursor_pos.x,cursor_pos.y,i,j);
                    moveCursor(i-cursor_pos.x,j-cursor_pos.y);  
                }          
                break;
            }
            i=NB_CELL-1;
            j=NB_CELL-1;
        }
    }
    else
    {
        printf("Find nxt pos\n");
        stop=0;
        j=cursor_pos.y;
        i=cursor_pos.x+1;
        while(1)
        {
            while(j<NB_CELL)
            {
                while(i<NB_CELL)
                {
                    if(allowedHuman[i][j]==1)
                    {
                        stop=1;
                        break;
                    }
                    i++;
                }
                if(stop)
                    break;
                i=0;
                j++;
            }            
        
            if(stop)
            {
                if(dispMove==DO_MOVE)
                {
                    printf("doing Move (%d,%d)->(%d,%d)\n",cursor_pos.x,cursor_pos.y,i,j);
                    moveCursor(i-cursor_pos.x,j-cursor_pos.y);
                }
                else
                {
                    printf("saving new pos (%d,%d)\n",cursor_pos.x,cursor_pos.y);
                    cursor_pos.x=i;
                    cursor_pos.y=j;
                }
                break;
            }
            i=0;
            j=0;
        }
    }
}

void iniCursorPos()
{
    printf("[inCursorPos] old:(%d,%d)\n",cursor_pos.x,cursor_pos.y);
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x=0;
    cursor_pos.y=0;    
    nxtCursosPos(NXT_POS,NO_MOVE);
    printf("[inCursorPos] new:(%d,%d)\n",cursor_pos.x,cursor_pos.y);
    selectCell(cursor_pos.x,cursor_pos.y);
}

int eventHandler(int evt)
{
    if(!endOfGame)
    {
        switch(evt)
        {
            case BTN_UP:
            case BTN_LEFT:
                nxtCursosPos(PREV_POS,DO_MOVE);
                break;                  
            case BTN_DOWN:
            case BTN_RIGHT:
                nxtCursosPos(NXT_POS,DO_MOVE);                       
                break;        
            case BTN_F1:
                doMove(cursor_pos.x,cursor_pos.y);
                computerMove();
                drawNbPiece();                      
#ifdef DEBUG
                printBoardAllowed();
#endif
                while(!computeAllowed(allowedHuman,HUMAN))
                {
                    if(!computerMove())
                    {
                        endOfGame=1;
                        endGame();
                        break;
                    }
                }
                if(!endOfGame)
                    iniCursorPos();
                break;     
        }
    }
    
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            RELEASE(cops)
            break;
        case BTN_ON:
            iniBoard();
            computeAllowed(allowedHuman,HUMAN);
        case EVT_REDRAW:
            drawBoard();
            drawMenu();
            iniCursorPos();
            drawNbPiece();
#ifdef DEBUG
            printBoardAllowed();
#endif
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
    computeAllowed(allowedHuman,HUMAN);
    eventHandler(EVT_REDRAW);
    PACK(cops,NULL);
    STOPME(cops);
    return 1;
}
