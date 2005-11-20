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

__SDRAM_DATA struct client_operations * cops;
struct position {
    int x;
    int y;
};

__SDRAM_DATA  struct position cursor_pos={3,2};

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

__SDRAM_DATA int stop_othello;

extern int board[NB_CELL][NB_CELL];
extern int nbPieces[2];
extern int allowedHuman[NB_CELL][NB_CELL];
extern void eventHandler(int evt);

__IRAM_CODE void unSelectCell(int x, int y);
__IRAM_CODE void selectCell(int x, int y);
__IRAM_CODE void nxtCursosPos(int direction,int dispMove);
__IRAM_CODE void moveCursor(int dx, int dy);
__IRAM_CODE void drawBoard(void);
__IRAM_CODE void drawMenu(void);
__IRAM_CODE void drawNbPiece(void);
__IRAM_CODE int computeAllowed(int allowed[NB_CELL][NB_CELL],int player);

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

inline void help_me(void)
{
    printf("help me\n");
}

__SDRAM_CODE void iniBoard(void)
{
    int i,j;

    printf("ini borad start\n");
    
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
    printf("ini borad end\n");
}

__SDRAM_CODE void endGame(void)
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

__SDRAM_CODE void iniCursorPos()
{
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x=0;
    cursor_pos.y=0;
    nxtCursosPos(NXT_POS,NO_MOVE);
    selectCell(cursor_pos.x,cursor_pos.y);
}



__SDRAM_CODE void redraw(void)
{
    printf("in redraw\n");
    drawBoard();
    drawMenu();
    iniCursorPos();
    drawNbPiece();
    printf("out redraw\n");
}


__SDRAM_CODE int _start(int argc,char* argv)
{
    //REGISTER(eventHandler,0);
    int evt;
    unsigned int evt_buffer;
    
    printf("\nIn othello\n");
    
    open_graphics();
  
    clearScreen(COLOR_WHITE);
    
    setFont(STD8X13);

    iniBoard();
    printf("back from ini board\n");
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
    return 0;
}

