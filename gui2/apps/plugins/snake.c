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
#include "graphics.h" /* graphics... */

#define MODE_INIT 1
#define MODE_PLAYING 2
#define MODE_PAUSED 3

#define DIR_UP    0
#define DIR_RIGHT 1
#define DIR_DOWN  2
#define DIR_LEFT  3

#define MAX_X 80
#define MAX_Y 56

/* Initialization, REQUIRED */
struct client_operations * cops;

static int board[MAX_X][MAX_Y], snakelength;
static unsigned int score;
static int dir,frames,apple,level=1;
int mode=1;
int wait=0, delay=0;
int appleX, appleY;

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
            cops->clearScreen(COLOR_BLACK);
            mode=MODE_INIT;
            break;
    }
    if(x==MAX_X || x<0 || y==MAX_Y || y<0)
    {
        cops->clearScreen(COLOR_BLACK);
        mode = MODE_INIT;
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
    cops->fillRect(COLOR_WHITE, x*4, y*4, 4, 4);
}

void frame(void)
{
    int x,y,head=0;
    for (x=0; x<MAX_X; x++)
    {
        for (y=0; y<MAX_Y; y++)
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

    for(x=0; x<MAX_X; x++)
    {
        for(y=0; y<MAX_Y; y++)
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
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 10,"SNAKE");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 25,"Game Paused");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 70,"Press [ON] to resume");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 3, 95,"Press [OFF] to quit current game");
}

void game(void)
{
    int x,y;
    char score_s[12];

    frame();
    frames++;
    if(frames==10)
    {
        frames=0;
        if(!apple)
        {
            do
            {
                srand(cops->getTick());
                x=rand() % MAX_X;
                y=rand() % MAX_Y;
            } while (board[x][y]);
            apple=1;
            board[x][y]=-1;
            appleX = x;
            appleY = y;
        }

        cops->fillRect(COLOR_WHITE, (appleX*4)+1, appleY*4, 2, 4);
        cops->fillRect(COLOR_WHITE, appleX*4, (appleY*4)+1, 4, 2);
    }

    sprintf(score_s,"Current score: %04d",score);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 226, score_s);

    /* no sleep() */
    while(wait < delay)
        wait++;
    wait = 0;
}

void game_init(void)
{
    int x,y;
    char plevel[10];
    /*char phscore[20];*/

    for (x=0; x<MAX_X; x++)
        for (y=0; y<MAX_Y; y++)
            board[x][y]=0;

    apple=0;
    snakelength=4;
    score=0;
    board[40][28]=1;

    sprintf(plevel,"Current Level: %02d",level);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 10, plevel);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 30, "1 is slowest, 10 is fastest");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 100, "Press [UP]/[DOWN] to change level");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 115, "Press [OFF] to quit plugin");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 2, 130, "Press [ON] to start/pause");
}

int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            if(mode == MODE_PLAYING || mode == MODE_PAUSED)
            {
                cops->clearScreen(COLOR_BLACK);
                mode = MODE_INIT;
            }
            else
                RELEASE(cops); /* we're done */
            break;

        case BTN_ON:
            if(mode == MODE_INIT)
            {
                cops->clearScreen(COLOR_BLACK);
                cops->drawLine(COLOR_WHITE, 0, 224, 320, 224);
                delay = (11-level)*2000;
                mode = MODE_PLAYING;
            }
            else if(mode == MODE_PLAYING)
            {
                cops->clearScreen(COLOR_BLACK);
                mode = MODE_PAUSED;
            }
            else if(mode == MODE_PAUSED)
            {
                cops->clearScreen(COLOR_BLACK);
                cops->drawLine(COLOR_WHITE, 0, 224, 320, 224);
                mode = MODE_PLAYING;
            }
            break;

        case BTN_UP:
            if(mode == MODE_INIT)
            {
                if(level<10)
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
                    level=10;
            }
            else if(mode == MODE_PLAYING)
                if (dir!=0) dir=2;
            break;

        case BTN_RIGHT:
            if(mode == MODE_INIT)
            {
                if(level<10)
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
                    level=10;
            }
            else if(mode == MODE_PLAYING)
                if(dir!=1) dir=3;
            break;
    }
    return 1;
}

void snake(void)
{
    switch(mode)
    {
        case MODE_INIT:    game_init();  break;
        case MODE_PLAYING: game();       break;
        case MODE_PAUSED:  game_pause(); break;
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
    return 1;
}
