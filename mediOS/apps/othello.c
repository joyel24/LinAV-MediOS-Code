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
#include <sys_def/string.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <api.h>
#include <evt.h>
#include <graphics.h>

struct client_operations * cops;
struct position {
    int x;
    int y;
};

struct position cursor_pos={3,2};

#define CELL_SIZE  20
#define PIECE_SIZE 12
#define NB_CELL    8
#define X_INI      10
#define Y_INI      ((240-((CELL_SIZE+1)*NB_CELL)-1)/2)

#define NB_PIECE_POS 90

#define BG_COLOR     COLOR_BLACK
#define LINE_COLOR   COLOR_WHITE
#define SEL_COLOR    COLOR_RED
#define TXT_COLOR    COLOR_WHITE

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
 int cursorMoveMode=0; /* 0 = move anywhere, 1 = move to possible locations */

 unsigned char human_bmap[12][12]= {
{00,00,00,00,24,24,24,24,00,00,00,00},
{00,00,24,24,24,24,24,24,24,24,00,00},
{00,24,24,24,24,24,24,24,24,24,24,00},
{00,24,24,24,00,00,00,00,24,24,24,00},
{24,24,24,00,00,00,00,00,00,24,24,24},
{24,24,24,00,00,00,00,00,00,24,24,24},
{24,24,24,00,00,00,00,00,00,24,24,24},
{24,24,24,00,00,00,00,00,00,24,24,24},
{00,24,24,24,00,00,00,00,24,24,24,00},
{00,24,24,24,24,24,24,24,24,24,24,00},
{00,00,24,24,24,24,24,24,24,24,00,00},
{00,00,00,00,24,24,24,24,00,00,00,00},
};

 unsigned char av3xx_bmap[12][12]= {
{00,00,00,00,25,25,25,25,00,00,00,00},
{00,00,25,25,25,25,25,25,25,25,00,00},
{00,25,25,25,25,25,25,25,25,25,25,00},
{00,25,25,25,25,25,25,25,25,25,25,00},
{25,25,25,25,25,25,25,25,25,25,25,25},
{25,25,25,25,25,25,25,25,25,25,25,25},
{25,25,25,25,25,25,25,25,25,25,25,25},
{25,25,25,25,25,25,25,25,25,25,25,25},
{00,25,25,25,25,25,25,25,25,25,25,00},
{00,25,25,25,25,25,25,25,25,25,25,00},
{00,00,25,25,25,25,25,25,25,25,00,00},
{00,00,00,00,25,25,25,25,00,00,00,00},
};

BITMAP hBitmap = {(unsigned int) human_bmap, 12, 12, 0, 0};
BITMAP aBitmap = {(unsigned int) av3xx_bmap, 12, 12, 0, 0};

int stop_othello;

inline void help_me(void)
{
    printf("help me\n");
}

/*
struct helperMenu othelloMenu = {
    ON_txt        : "New Game",
    OFF_txt       : "Quit",
    JOY_txt       : NULL,
    F1_txt        : "Play",
    F2_txt        : NULL,
    F3_txt        : "Nav mode",
    
    helperDelay   :  1,
    helperSpeed   :  300,
    
    bg_color      : COLOR_WHITE,
    border_color  : COLOR_BLUE,
    txt_color     : COLOR_BLACK,
    
    align         : ALIGN_RIGHT
};
*/
#define PIECE_OFFSET (CELL_SIZE-PIECE_SIZE)/2+1


 void drawPiece(int x, int y, int player)
{
    switch(player)
    {
        case HUMAN:
            drawBITMAP(&hBitmap,X_INI+PIECE_OFFSET+x*(CELL_SIZE+1),Y_INI+PIECE_OFFSET+y*(CELL_SIZE+1));
            break;
        case AV3XX:
            drawBITMAP(&aBitmap,X_INI+PIECE_OFFSET+x*(CELL_SIZE+1),Y_INI+PIECE_OFFSET+y*(CELL_SIZE+1));
            break;
    }
}

 void erasePiece(int x, int y)
{
    fillRect(BG_COLOR,X_INI+1+x*(CELL_SIZE+1),Y_INI+1+y*(CELL_SIZE+1),CELL_SIZE,CELL_SIZE);
}

 void drawBoard(void)
{
    int i,j;

    clearScreen(BG_COLOR);


    for(i=0;i<NB_CELL+1;i++)
    {
        /* horizontal line */
        drawLine(LINE_COLOR,X_INI,Y_INI+i*(CELL_SIZE+1),X_INI+NB_CELL*(CELL_SIZE+1),Y_INI+i*(CELL_SIZE+1));
        /* vertical line */
        drawLine(LINE_COLOR,X_INI+i*(CELL_SIZE+1),Y_INI,X_INI+i*(CELL_SIZE+1),Y_INI+NB_CELL*(CELL_SIZE+1));
    }

    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
            if(board[i][j]!=EMPTY)
                drawPiece(i, j,board[i][j]);
}

 void selectCell(int x, int y)
{
    drawRect(SEL_COLOR,X_INI+x*(CELL_SIZE+1),Y_INI+y*(CELL_SIZE+1),CELL_SIZE+2,CELL_SIZE+2);
}

 void unSelectCell(int x, int y)
{
    drawRect(LINE_COLOR,X_INI+x*(CELL_SIZE+1),Y_INI+y*(CELL_SIZE+1),CELL_SIZE+2,CELL_SIZE+2);
}

 void drawNbPiece()
{
    char tmp[10];

    sprintf(tmp,"You: %02d",nbPieces[HUMAN]);
    putS(TXT_COLOR, BG_COLOR, 20, 220, tmp);
    drawBITMAP(&hBitmap, 4, 220);

    sprintf(tmp,"Archos: %02d",nbPieces[AV3XX]);
    putS(TXT_COLOR, BG_COLOR, 120, 220, tmp);
    drawBITMAP(&aBitmap, 104, 220);
}

 void drawMenu()
{
    int w=0,h=0;

    help_me();
    
    getStringS("Nav Mode:", &w, &h);
    putS(TXT_COLOR, BG_COLOR, 320-w-5,210, "Nav Mode:");

    if(cursorMoveMode == 0)
    {
        getStringS("Traditional", &w, &h);
        putS(TXT_COLOR, BG_COLOR, 320-w-5,225, "Traditional");
    }
    else
    {
        getStringS("Standard   ", &w, &h);
        putS(TXT_COLOR, BG_COLOR, 320-w-5,225, "Standard   ");
    }
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

    /* setting initial piece */
    board[i][i]=AV3XX;board[i+1][i]=HUMAN;
    board[i][i+1]=HUMAN;board[i+1][i+1]=AV3XX;

    /* reset scores */
    nbPieces[1] = 2;
    nbPieces[2] = 2;
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
    int w,h;

    getStringS("Game Over... ",&w,&h);

    putS(COLOR_WHITE,COLOR_BLACK,10,2,"Game Over...");

    if(nbPieces[HUMAN]>nbPieces[AV3XX])
        putS(COLOR_WHITE,COLOR_BLACK,10+w,2,"You won! :D");
    else if(nbPieces[HUMAN]<nbPieces[AV3XX])
        putS(COLOR_WHITE,COLOR_BLACK,10+w,2,"You lost. :(");
    else
        putS(COLOR_WHITE,COLOR_BLACK,10+w,2,"You tied. :|");
}

 void nxtCursosPos(int direction,int dispMove)
{
    int i,j;
    int stop;

    if(direction == PREV_POS)
    {
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
                    moveCursor(i-cursor_pos.x,j-cursor_pos.y);
                else
                {
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
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x=0;
    cursor_pos.y=0;
    nxtCursosPos(NXT_POS,NO_MOVE);
    selectCell(cursor_pos.x,cursor_pos.y);
}

 void simpleMove(int dx,int dy)
{
    int cx = cursor_pos.x;
    int cy = cursor_pos.y;

    if(cx+dx>=0 && cy+dy>=0 && cx+dx<=NB_CELL-1 && cy+dy<=NB_CELL-1)
        moveCursor(dx,dy);
}



 void eventHandler(int evt)
{  
    switch(evt)
    {
        case BTN_UP:
            if(cursorMoveMode==0)
                simpleMove(0,-1);
            break;

        case BTN_LEFT:
            if(cursorMoveMode==0)
                simpleMove(-1,0);
            else
                nxtCursosPos(PREV_POS,DO_MOVE);
            break;

        case BTN_DOWN:
            if(cursorMoveMode==0)
                simpleMove(0,1);
            break;

        case BTN_RIGHT:
            if(cursorMoveMode==0)
                simpleMove(1,0);
            else
                nxtCursosPos(NXT_POS,DO_MOVE);
            break;

        case BTN_F1:
            if(cursorMoveMode==0 && allowedHuman[cursor_pos.x][cursor_pos.y]!=1)
                break;

            doMove(cursor_pos.x,cursor_pos.y);
            computerMove();
            drawNbPiece();
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

        case BTN_F3:
            cursorMoveMode = !cursorMoveMode;
            drawMenu();
            break;

        case BTN_OFF:
        case EVT_QUIT:
            stop_othello=1;
            break;

        case BTN_ON:
            iniBoard();
            computeAllowed(allowedHuman,HUMAN);
            break;

        case EVT_REDRAW:
            redraw();
            break;
    }
}

 void redraw(void)
{
    drawBoard();
    drawMenu();
    iniCursorPos();
    drawNbPiece();
}

void _start(void)
{
    int evt;
    unsigned int evt_buffer;
    
    printf("\nIn othello\n");
    
    open_graphics();
   
    clearScreen(COLOR_WHITE);
    
    setFont(STD8X13);

    iniBoard();
    

    evt_buffer=get_evt_pipe();
    if(!evt_buffer)
    {
        printf("[ini_status_bar] can't register to evt\n");
    }
    
    computeAllowed(allowedHuman,HUMAN);
    redraw();
 
    stop_othello=0;
    printf("\nbefore loop\n");
    while(!stop_othello)
    {
        evt=get_evt(evt_buffer);
        eventHandler(evt);
    }
    printf("\nafter loop\n");
    rm_evt_pipe(evt_buffer);

    printf("\nout othello\n");
    
}


