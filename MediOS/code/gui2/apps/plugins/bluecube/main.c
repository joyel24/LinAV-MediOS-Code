/*
 * Bluecube for Av3XX
 * removed all SDL specific code
 *
 * BlueCube - just another tetris clone 
 * Copyright (C) 2003  Sebastian Falbesoner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "graphics.h"
#include "events.h"
#include "colordef.h"
#include "font.h"

#include "cops.h"
#include "avevents.h"

#include "global.h"
#include "grafix.h"
#include "box.h"


static void NewGame(void);
//static void MainMenu_Loop(void);
static void Game_Loop(void);
static void DrawScene(void);
void StartGameOverAnimation(void);
static void GameOverAnimation(void);
void ClearCluster(void);


int lines, score;       /* Line counter and score */
int nextPiece;          /* Next cluster  */
int level;              /* Current level */

int zustand;     /* Current state */
int bDone=0;     /* Want to Exit? */
int bPause;      /* Pause? */
int bCrazy;      /* Yahooooooooooo ;) CRAZY MODE ^.^ */
int bGameOver;   /* GameOver Animation? */
int bExplode;    /* Explosion is active? */
int x,y;         /* Current explosion coordinates */

struct client_operations * cops;



int evt=-1;

int eventHandler(int event)
{   
	
	switch (event)
        {	
            /*case EVT_TIMER:
                cluster.dropCount--;
                break;*/
            case BTN_DOWN:                
            case BTN_UP:
            case BTN_RIGHT:
            case BTN_LEFT:
            case BTN_F1:
                if(evt==-1)
                    evt=event;
                break;
            case BTN_OFF:
            case EVT_QUIT:
                RELEASE(cops)
                break;
        }
        
        /*Game_Loop();
        if(bDone == 1)
            RELEASE(cops)*/
        
	return 1;
}

void processEvt(void)
{
        switch (evt)
        {	
            /*case EVT_TIMER:
                cluster.dropCount--;
                break;*/
            case BTN_DOWN:
                if(MoveCluster(0))
                    NewCluster();
                break;
            case BTN_UP:
                TurnClusterRight();
                break;
            case BTN_RIGHT:
                MoveClusterRight();
                break;
            case BTN_LEFT:
                MoveClusterLeft();
                break;
            case BTN_F1:
                MoveCluster(1); /* "drop" cluster...      */
                NewCluster();   /* ... and create new one */
                break;
        }
        evt=-1;
}

void mainLoop(void)
{
    int i=0;
    
    while(i<100 )
    {
        i++;
    
        if(evt!=-1)
        {
            ClearCluster(); 
            processEvt();
         }
    }   
    
     
    cluster.dropCount--;  /* Decrease time until the next fall */;

    Game_Loop();
    if(bDone == 1)
        RELEASE(cops)
}

int main(int argc,char** argv)
{
    REGISTER(cops,eventHandler,0);
    srand(cops->getTick());
    cops->hideSBar();  
    cops->disableMenu();
    
    cops->clearScreen(COLOR_WHITE);    
    
    /* intialise game state */
    NewGame();

    PACK(cops,mainLoop);
    STOPME(cops);
    return 1;
    
}


/*=========================================================================
// Name: NewGame()
// Desc: Starts a new game
//=======================================================================*/
static void NewGame()
{
	InitBox(); /* Clear Box */
	BoxDrawInit(); /* Init boxdraw values */
	
	lines = 0; /* Reset lines counter */
	score = 0; /* Reset score */
	level = 0; /* Reset level */
	nextPiece = rand()%7; /* Create random nextPiece */
	NewCluster();

	bPause = 0; /* No pause */
	bCrazy = 0; /* No crazymode :) */
	bGameOver = 0;

	zustand = STATE_PLAY; /* Set playstate */
}

/*=========================================================================
// Name: Game_Loop()
// Desc: The main loop for the game
//=======================================================================*/
static void Game_Loop()
{
	if (!bPause)
	{
		if (!bGameOver)
		{
			if (cluster.dropCount == 0)
			{
                            ClearCluster(); 
				if (MoveCluster(0)) /* If cluster "collides"... */
                                {
					NewCluster();   /* then create a new one ;) */
                                        cops->putS(COLOR_BLACK,COLOR_WHITE, 155, 24, "Next");
                                        cops->fillRect(COLOR_WHITE,160,40,boxdraw.brick_width*CLUSTER_X+(CLUSTER_X-1)*boxdraw.box_l
                                ,boxdraw.brick_height*CLUSTER_Y+(CLUSTER_Y-1)*boxdraw.box_l);
		DrawNextPiece(160, 40);
                                }
			}

			/* Increase Level */
			if (((level == 0) && (lines >=  10)) ||
				((level == 1) && (lines >=  20)) ||
				((level == 2) && (lines >=  40)) ||
				((level == 3) && (lines >=  80)) ||
				((level == 4) && (lines >= 100)) ||
				((level == 5) && (lines >= 120)) ||
				((level == 6) && (lines >= 140)) ||
				((level == 7) && (lines >= 160)) ||
				((level == 8) && (lines >= 180)) ||
				((level == 9) && (lines >= 200)))
			{
				level++;
			}
		}
		else
		{
			GameOverAnimation();
		}

	}

	DrawScene();
}

/*=========================================================================
// Name: DrawScene()
// Desc: Draws the whole scene!
//=======================================================================*/
static void DrawScene()
{
	char chScore[30], chLines[30],chLevel[30];


	if (!bGameOver)
	{
		/* Draw border of the box */
		PutRect(boxdraw.box_x-5, boxdraw.box_y-5,
				boxdraw.box_width + 2*5, boxdraw.box_height + 2*5, COLOR_BLACK,0,0);
		PutRect(boxdraw.box_x-1, boxdraw.box_y-1,
				boxdraw.box_width+2, boxdraw.box_height+2, COLOR_BLACK,0,0);

		DrawCluster(); /* Draw cluster */
		sprintf(chScore, "%d", score);
		sprintf(chLines, "%d", lines);
		sprintf(chLevel, "%d", level);
		
		cops->putS(COLOR_BLACK,COLOR_WHITE, 1, 20, "Score");
		cops->putS(COLOR_BLACK,COLOR_WHITE, 1, 32, chScore);
		cops->putS(COLOR_BLACK,COLOR_WHITE, 155, 24, "Next");
                
                
		
		cops->putS(COLOR_BLACK,COLOR_WHITE, 1, 45, "Lines");
		cops->putS(COLOR_BLACK,COLOR_WHITE, 1, 57, chLines);
		cops->putS(COLOR_BLACK,COLOR_WHITE, 1, 72, "Level");
		cops->putS(COLOR_BLACK,COLOR_WHITE, 1, 84, chLevel);
	}

	DrawBox();       /* Draw box bricks */
	if (bGameOver && !bExplode)
	{
		/*GrSetGCForeground(tetris_gc, BLACK);
		GrFillRect(tetris_wid, tetris_gc, 0, 0, 168, 128);
		GrSetGCForeground(tetris_gc, WHITE);
		GrText(tetris_wid, tetris_gc, 45, 35, "- Game Over -",  -1, GR_TFASCII);        sprintf(chScore, "Your Score: %d", score);
		GrText(tetris_wid, tetris_gc, 35, 65, chScore,  -1, GR_TFASCII);*/
                cops->clearScreen(COLOR_WHITE);
	}

}

/*=========================================================================
// Name: StartGameOverAnimation()
// Desc: Starts the gameover animation
//=======================================================================*/
void StartGameOverAnimation()
{
	x = 0;
	y = 0;
	bGameOver = 1;
}

/*=========================================================================
// Name: GameOverAnimation()
// Desc: Gameover Animation!
//=======================================================================*/
static void GameOverAnimation()
{
}

/*=========================================================================
// Name: PutRect()
// Desc: Draws a /UN/ filled rectangle onto the screen
//=======================================================================*/
void PutRect(int x, int y, int w, int h, int r, int g, int b)
{
	// GrSetGCForeground(tetris_gc, MWRGB(r, g, b));
	cops->drawRect(r,x,y,w,h);
}

void ClearRect(int x, int y, int w, int h)
{
    cops->drawRect(COLOR_WHITE,x,y,w,h);
}

void ClearBox(void)
{
    cops->fillRect(COLOR_WHITE,boxdraw.box_x+1,boxdraw.box_y+1, boxdraw.box_width-1, boxdraw.box_height-1);
}


