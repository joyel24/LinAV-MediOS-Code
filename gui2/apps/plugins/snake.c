/*
* snake.c
*
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2002 Itai Shaked
* Ported to LinAV from Rockbox project by Zakk Roberts
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

/* general stuff */
#include <stdio.h>
#include <stdlib.h>

#include "cops.h" /* for plugin API functions */
#include "avevents.h" /* for events */
#include "events.h" /* for events */
#include "font.h" /* for font definitions/switching */
#include "colordef.h" /* for color definitions (COLOR_BLACK, etc) */

#define MODE_INIT 1
#define MODE_PLAYING 2
#define MODE_PAUSED 3
#define MODE_DEAD 4
int mode = 1;

/* Initialization, REQUIRED */
struct client_operations * cops;

static int board[28][16],snakelength;
static unsigned int score,hiscore=0;
static int dir,frames,apple,level=1,dead=0;
int wait = 0;

void die(void)
{
    char pscore[5],hscore[17];
    cops->clearScreen(COLOR_BLACK);

    sprintf(pscore,"Your Score: %d",score);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 10, "Oops!");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 30, pscore);

    if (score>hiscore)
    {
        hiscore=score;
        cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 45, "New High Score!");
    }
    else
    {
        sprintf(hscore,"High Score: %d",hiscore);
        cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 45,hscore);
    }
    dead=1;
}

void collision(int x, int y)
{
    switch (board[x][y])
    {
        case 0:
            break;
        case -1:
            snakelength+=2;
            score+=level;
            apple=0;
            break;
        default:
            die();
            break;
    }
    if (x==28 || x<0 || y==16 || y<0)
        die();
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
    if(dead)
        return;
    board[x][y]=1;
    cops->fillRect(COLOR_WHITE, x*4, y*4, 4, 4);
}

void frame(void)
{
    int x,y,head=0;
    for (x=0; x<28; x++)
    {
        for (y=0; y<16; y++)
        {
            switch (board[x][y])
            {
                case 1:
                    if (!head) {
                        move_head(x,y);
                        if (dead)
                            return;
                        board[x][y]++;
                        head=1;
                    }
                    break;
                case 0:
                    break;
                case -1:
                    break;
                default:
                    if (board[x][y]==snakelength) {
                        board[x][y]=0;
                        cops->fillRect(COLOR_BLACK, x*4, y*4, 4, 4);
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
    cops->clearScreen(COLOR_BLACK);
    for (x=0; x<28; x++)
    {
        for (y=0; y<16; y++)
        {
            switch (board[x][y])
            {
                case -1:
                    cops->fillRect(COLOR_WHITE, (x*4)+1, y*4, 2, 4);
                    cops->fillRect(COLOR_WHITE, x*4, (y*4)+1, 4, 2);
                    break;
                case 0:
                    break;
                default:
                    cops->fillRect(COLOR_WHITE, x*4, y*4, 4, 4);
                    break;
            }
        }
    }
}

void game_pause(void)
{
    cops->clearScreen(COLOR_BLACK);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 10,"Game Paused");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 30,"[On] to resume");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 50,"[Off] to quit");
}

void game(void)
{
    int x,y;

    frame();
    if (dead)
        return;
    frames++;
    if(frames==10)
    {
        frames=0;
        if(!apple)
        {
            do
            {
                x=rand() % 28;
                y=rand() % 16;
            } while (board[x][y]);
            apple=1;
            board[x][y]=-1;
            cops->fillRect(COLOR_WHITE, (x*4)+1, y*4, 2, 4);
            cops->fillRect(COLOR_WHITE, x*4, (y*4)+1, 4, 2);
        }
    }

    /* no sleep() */
    while(wait < level*100)
        wait++;
    wait = 0;
}

void game_init(void)
{
    int x,y;
    char plevel[10],phscore[20];

    for (x=0; x<28; x++)
        for (y=0; y<16; y++)
            board[x][y]=0;
    dead=0;
    apple=0;
    snakelength=4;
    score=0;
    board[11][7]=1;

    sprintf(plevel,"Level - %02d",level);
    sprintf(phscore,"High Score: %d",hiscore);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 10, plevel);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 30, "(1-slow, 9-fast)");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 60, "OFF = quit");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 90, "ON = start/pause");
}

int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            RELEASE(cops); /* we're done */
            break;

        case BTN_ON:
            if(mode == MODE_INIT)
                mode = 2;
            else if(mode == MODE_PLAYING)
                mode = 3;
            else if(mode == MODE_PAUSED)
                mode = 2;
            else if(mode == MODE_DEAD)
                mode = 1;
            break;

        case BTN_UP:
            if(mode == MODE_INIT)
            {
                if(level<9)
                    level++;
                else
                    level=1;
            }
            else if(mode == MODE_PLAYING)
                 if (dir!=2) dir=0;
            break;

        case BTN_DOWN:
            if(mode == MODE_INIT)
            {
                if(level>1)
                    level--;
                else
                    level=9;
            }
            else if(mode == MODE_PLAYING)
                if (dir!=0) dir=2;
            break;

        case BTN_RIGHT:
            if(mode == MODE_INIT)
            {
                if(level<9)
                    level++;
                else
                    level=1;
            }
            else if(mode == MODE_PLAYING)
                if (dir!=3) dir=1;
            break;

        case BTN_LEFT:
            if(mode == MODE_INIT)
            {
                if(level>1)
                    level--;
                else
                    level=9;
            }
            else if(mode == MODE_PLAYING)
                if (dir!=1) dir=3;
            break;
    }
}

void snake(void)
{
    switch(mode)
    {
        case 1: game_init();  break;
        case 2: game();       break;
        case 3: game_pause(); break;
        case 4: die();        break;
    }
}

int main(int argc,char * * argv)
{
    /* Initialization, REQUIRED */
    REGISTER(cops,eventHandler,0);

    cops->hideSBar(); /* shut off the status bar */
    cops->disableMenu(); /* disable the F3=MENU function */
    cops->clearScreen(COLOR_BLACK); /* clear the LCD to black */
    cops->setFont(STD8X13);

    /* These two functions REQUIRED */
    /* Usage: PACK(cops, <main function>)*/
    PACK(cops,snake);
    /* Stay here in the plugin */
    STOPME(cops);
}
