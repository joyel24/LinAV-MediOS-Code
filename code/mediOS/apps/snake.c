/*
*   spuzzle.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Ported to LinAV from Rockbox project by Zakk Roberts
* 
*
*/

#include <api.h>
#include <evt.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#define MODE_INIT 1
#define MODE_PLAYING 2
#define MODE_PAUSED 3

#define DIR_UP    0
#define DIR_RIGHT 1
#define DIR_DOWN  2
#define DIR_LEFT  3

#define STATUS_Y (screenHeight-fontHeight)

int screenWidth,screenHeight,fontHeight,arch;
int maxX,maxY;

static int board[100][100], snakelength;
static unsigned int score=0;
static int dir,frames,apple,level=1;
int mode=1;
int wait=0, delay=0;
int appleX, appleY;

int evt_handler;

int collision_done=0;

void snake(void);

void collision(int x, int y)
{
    switch(board[x][y])
    {
        case 0:
            break;
        case -1:
            snakelength+=2;
            score+=level;
            apple=0;
            break;
        default:
            collision_done=1;
            break;
    }
    if(x==maxX || x<0 || y==maxY || y<0)
    {
        collision_done=1;
    }
}

void move_head(int x, int y)
{
    switch(dir)
    {
        case 0:
            y-=1;
            break;
        case 1:
            x+=1;
            break;
        case 2:
            y+=1;
            break;
        case 3:
            x-=1;
            break;
    }
    collision(x,y);
    board[x][y]=1;
    gfx_fillRect(COLOR_WHITE, x*4, y*4, 4, 4);
}

void frame(void)
{
    int x,y,head=0;
    for (x=0; x<maxX; x++)
    {
        for (y=0; y<maxY; y++)
        {
            switch(board[x][y])
            {
                case 1:
                    if (!head)
                    {
                        move_head(x,y);
                        board[x][y]++;
                        head=1;
                    }
                    break;
                case 0:
                    break;
                case -1:
                    break;
                default:
                    if (board[x][y]==snakelength)
                    {
                        board[x][y]=0;
                        gfx_fillRect(COLOR_BLACK, x*4, y*4, 4, 4);
                    }
                    else
                        board[x][y]++;
                    break;
            }
        }
    }
}

void redraw(void)
{
    int x,y;

    gfx_clearScreen(COLOR_BLACK);

    for(x=0; x<maxX; x++)
    {
        for(y=0; y<maxY; y++)
        {
            switch (board[x][y])
            {
                case -1:
                    gfx_fillRect(COLOR_WHITE, (x*4)+1, y*4, 2, 4);
                    gfx_fillRect(COLOR_WHITE, x*4, (y*4)+1, 4, 2);
                    break;
                case 0:
                    break;
                default:
                    gfx_fillRect(COLOR_WHITE, x*4, y*4, 4, 4);
                    break;
            }
        }
    }
}

int game_pause(void)
{
    int evt;
    
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 3, 10,"SNAKE");
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 3, 25,"Game Paused");
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 3, 70,"Press [ON] to resume");
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 3, 95,"Press [OFF] to quit current game");
    
    printf("In pause\n");
    
    while(1)
    {
        evt=evt_getStatus(evt_handler);        
        if(evt==NO_EVENT)
            continue;
        switch(evt)
        {
            case BTN_OFF:
                return 0;                
                    
            case BTN_ON:
                return 1;
        }
    }
    return 0;
}

void game(void)
{
    int x,y;
    char score_s[30];

    frame();
    frames++;
    if(frames==10)
    {
        frames=0;
        if(!apple)
        {
            do
            {
                srand(tmr_getTick());
                x=rand() % maxX;
                y=rand() % maxY;
            } while (board[x][y]);
            apple=1;
            board[x][y]=-1;
            appleX = x;
            appleY = y;
        }

        gfx_fillRect(COLOR_WHITE, (appleX*4)+1, appleY*4, 2, 4);
        gfx_fillRect(COLOR_WHITE, appleX*4, (appleY*4)+1, 4, 2);
    }

    sprintf(score_s,"Current score: %04d",score);
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, STATUS_Y, score_s);

    mdelay(delay);
}

int game_init(void)
{
    int x,y;
    char plevel[30];
    char score_s[20];
    int redraw=1;
    int evt;
    /*char phscore[20];*/

    for (x=0; x<maxX; x++)
        for (y=0; y<maxY; y++)
            board[x][y]=0;

    apple=0;
    snakelength=4;
    
    board[40][28]=1;
    
    collision_done=0;
    
    printf("In init\n");
    
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, 30, "1 is slowest, 10 is fastest");
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, 100, "Press [UP]/[DOWN] to change level");
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, 115, "Press [OFF] to quit plugin");
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, 130, "Press [ON] to start/pause");
    sprintf(score_s,"Score: %04d",score);
    gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, STATUS_Y, score_s);
    
    while(1)
    {
        
        if(redraw)
        {
            sprintf(plevel,"Current Level: %02d",level);    
            gfx_putS(COLOR_WHITE, COLOR_BLACK, 2, 10, plevel);
            redraw=0;
        }
        
        evt=evt_getStatus(evt_handler);
        if(evt==NO_EVENT)
            continue;

        switch(evt)
        {
            case BTN_OFF:
                return 0;                
                    
            case BTN_ON:
                    gfx_clearScreen(COLOR_BLACK);
                    gfx_drawLine(COLOR_WHITE, 0, STATUS_Y-1, screenWidth, STATUS_Y-1);
                    delay = (11-level)*15;
                    score=0;
                    return 1;
            case BTN_UP:                    
                    if(level<10)
                    {
                        level++;
                        redraw=1;
                    }
                    else
                        level=1;                    
                    break;    
            case BTN_DOWN:
                    if(level>1)
                    {
                        level--;
                        redraw=1;
                    }
                    else
                        level=10;
                    break;
            case BTN_RIGHT:
                    if(level<10)
                    {
                        level++;
                        redraw=1;
                    }
                    else
                        level=1;                    
                    break;    
            case BTN_LEFT:
                    if(level>1)
                    {
                        level--;
                        redraw=1;
                    }
                    else
                        level=10;
                    break;
        }
    }
    return 0;
}

void eventHandlerLoop(void)
{
    int stop=0;
    int evt=0;
    
    printf("In evt loop\n");
    
    while(!stop)
    {
        evt=evt_getStatus(evt_handler);
        if(evt!=NO_EVENT)
        {
            switch(evt)
            {
                case BTN_OFF:
                    if(!game_init())
                        stop=1;
                    break;
                case BTN_ON:
                    gfx_clearScreen(COLOR_BLACK);
                    if(!game_pause())
                    {
                        stop=1;
                        break;
                    }
                    gfx_clearScreen(COLOR_BLACK);
                    gfx_drawLine(COLOR_WHITE, 0, STATUS_Y-1, screenWidth, STATUS_Y-1);
                    break;
                case BTN_UP:
                    if (dir!=2) dir=0;
                    break;
                case BTN_DOWN:
                    if (dir!=0) dir=2;
                    break;
                case BTN_RIGHT:
                    if (dir!=3) dir=1;
                    break;
                case BTN_LEFT:
                    if(dir!=1) dir=3;
                    break;
            }
        }
        game();
        if(collision_done)
        {
            gfx_clearScreen(COLOR_BLACK);
            if(!game_init())
                stop=1;
        }
    }
    printf("out evt loop\n");
}



void arch_init(){
    arch=getArch();
    getResolution(&screenWidth,&screenHeight);

    if(screenWidth>=320){
        gfx_fontSet(STD7X13);
        fontHeight=13;
    }else{
        gfx_fontSet(STD6X9);
        fontHeight=9;
    }

    maxX=screenWidth/4;
    maxY=(STATUS_Y-1)/4;
}

void app_main(int argc,char * * argv)
{
    gfx_clearScreen(COLOR_BLACK); /* clear the LCD to black */

    arch_init();

    evt_handler=evt_getHandler(BTN_CLASS);
    if(game_init())
        eventHandlerLoop();
    evt_freeHandler(evt_handler);
}

