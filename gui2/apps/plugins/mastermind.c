/*
 * mastermind.c
 * by Schoki 2004
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
#include "stdlib.h"
#include "stdio.h"

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"
#include "font.h"

#define true 1
#define false 0
#define X_OFFSET 80
#define X_OFFSET_BALLS 10
#define Y_OFFSET 40
#define ROWS 8
#define Y_STEP 18
#define LENGTH 16

/* here is a global api struct pointer. while not strictly necessary,
   it's nice not to have to pass the api pointer in all function calls
   in the plugin */
struct client_operations * cops;

void drawPieces();
void showResult();
void setCursor(int position, int state);

/* the cursor coordinates */
int turns = 0;
int pos = 0;
int piece = 0;

int pieceValues[4]  = { -1, -1, -1, -1 };
int resultValues[4] = { -1, -1, -1, -1 };

unsigned char clear[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char red[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15},
      {15,15,15,15,15,12,12,12,12,12,12,15,15,15,15,15},
      {15,15,15,15,12,12,12,12,12,12,12,12,15,15,15,15},
      {15,15,15,12,12,12,12,12,12,12,12,12,12,15,15,15},
      {15,15,15,12,12,12,12,12,12,12,12,12,12,15,15,15},
      {15,15,15,15,12,12,12,12,12,12,12,12,15,15,15,15},
      {15,15,15,15,15,12,12,12,12,12,12,15,15,15,15,15},
      {15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char yellow[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,14,14,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,14,14,14,14,15,15,15,15,15,15},
      {15,15,15,15,15,14,14,14,14,14,14,15,15,15,15,15},
      {15,15,15,15,14,14,14,14,14,14,14,14,15,15,15,15},
      {15,15,15,14,14,14,14,14,14,14,14,14,14,15,15,15},
      {15,15,15,14,14,14,14,14,14,14,14,14,14,15,15,15},
      {15,15,15,15,14,14,14,14,14,14,14,14,15,15,15,15},
      {15,15,15,15,15,14,14,14,14,14,14,15,15,15,15,15},
      {15,15,15,15,15,15,14,14,14,14,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,14,14,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char grey[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 7, 7,15,15,15,15,15,15,15},
      {15,15,15,15,15,15, 7, 7, 7, 7,15,15,15,15,15,15},
      {15,15,15,15,15, 7, 7, 7, 7, 7, 7,15,15,15,15,15},
      {15,15,15,15, 7, 7, 7, 7, 7, 7, 7, 7,15,15,15,15},
      {15,15,15, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,15,15,15},
      {15,15,15, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,15,15,15},
      {15,15,15,15, 7, 7, 7, 7, 7, 7, 7, 7,15,15,15,15},
      {15,15,15,15,15, 7, 7, 7, 7, 7, 7,15,15,15,15,15},
      {15,15,15,15,15,15, 7, 7, 7, 7,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 7, 7,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char green[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 2, 2,15,15,15,15,15,15,15},
      {15,15,15,15,15,15, 2, 2, 2, 2,15,15,15,15,15,15},
      {15,15,15,15,15, 2, 2, 2, 2, 2, 2,15,15,15,15,15},
      {15,15,15,15, 2, 2, 2, 2, 2, 2, 2, 2,15,15,15,15},
      {15,15,15, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,15,15,15},
      {15,15,15, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,15,15,15},
      {15,15,15,15, 2, 2, 2, 2, 2, 2, 2, 2,15,15,15,15},
      {15,15,15,15,15, 2, 2, 2, 2, 2, 2,15,15,15,15,15},
      {15,15,15,15,15,15, 2, 2, 2, 2,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 2, 2,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char blue[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 9, 9,15,15,15,15,15,15,15},
      {15,15,15,15,15,15, 9, 9, 9, 9,15,15,15,15,15,15},
      {15,15,15,15,15, 9, 9, 9, 9, 9, 9,15,15,15,15,15},
      {15,15,15,15, 9, 9, 9, 9, 9, 9, 9, 9,15,15,15,15},
      {15,15,15, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,15,15,15},
      {15,15,15, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,15,15,15},
      {15,15,15,15, 9, 9, 9, 9, 9, 9, 9, 9,15,15,15,15},
      {15,15,15,15,15, 9, 9, 9, 9, 9, 9,15,15,15,15,15},
      {15,15,15,15,15,15, 9, 9, 9, 9,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 9, 9,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char white[8][8] =
    { {15,15,15,15,15,15,15,15},
      {15,00,00,00,00,00,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,00,00,00,00,00,15},
      {15,15,15,15,15,15,15,15}
    };

unsigned char black[8][8] =
    { {15,15,15,15,15,15,15,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,15,15,15,15,15,15,15}
    };

unsigned char empty[8][8] =
    { {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,00,00,15,15,15},
      {15,15,15,00,00,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15}
    };

static BITMAP  clearB =  {(unsigned int)  clear,  16, 16, 0, 0};
static BITMAP    redB =  {(unsigned int)    red,  16, 16, 0, 0};
static BITMAP yellowB =  {(unsigned int) yellow,  16, 16, 0, 0};
static BITMAP   greyB =  {(unsigned int)   grey,  16, 16, 0, 0};
static BITMAP  greenB =  {(unsigned int)  green,  16, 16, 0, 0};
static BITMAP   blueB =  {(unsigned int)   blue,  16, 16, 0, 0};

static BITMAP  blackB =  {(unsigned int)  black,   8,  8, 0, 0};
static BITMAP  whiteB =  {(unsigned int)  white,   8,  8, 0, 0};
static BITMAP  emptyB =  {(unsigned int)  empty,   8,  8, 0, 0};

void delay(int value)
{
    int i,j,k;

    for(i=0; i<value;i++)
        for(j=0; j<value;j++)
            for(k=0; k<value;k++){}
}

void WinGame()
{
    cops->setFont(STD8X13);
    cops->putS(COLOR_BLACK, COLOR_WHITE, 70,90, "!!! Y O U  W I N !!!");
    cops->setFont(STD6X9);
}

void LooseGame()
{
    showResult();

    cops->setFont(STD8X13);
    cops->putS(COLOR_BLACK, COLOR_WHITE, 70,90, "!!! Y O U  L O O S E !!!");
    cops->setFont(STD6X9);
}

void initMatrix()
{
    int i = 0;

    for(i = 0; i < ROWS; i++)
    {
        // Auswertefeld
        cops->drawBITMAP (&emptyB, X_OFFSET, Y_OFFSET + i*Y_STEP);                       // links oben
        cops->drawBITMAP (&emptyB, X_OFFSET, Y_OFFSET + i*Y_STEP + LENGTH/2);            // links unten
        cops->drawBITMAP (&emptyB, X_OFFSET + LENGTH/2, Y_OFFSET + i*Y_STEP);            // rechts oben
        cops->drawBITMAP (&emptyB, X_OFFSET + LENGTH/2, Y_OFFSET + i*Y_STEP + LENGTH/2); // rechts unten

        // Schwarze Kugeln
        cops->drawBITMAP (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS +   LENGTH, Y_OFFSET + i*Y_STEP);
        cops->drawBITMAP (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS + 2*LENGTH, Y_OFFSET + i*Y_STEP);
        cops->drawBITMAP (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS + 3*LENGTH, Y_OFFSET + i*Y_STEP);
        cops->drawBITMAP (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS + 4*LENGTH, Y_OFFSET + i*Y_STEP);
    }
}

void generate()
{
    resultValues[0] = rand() % 5;
    resultValues[1] = rand() % 5;
    resultValues[2] = rand() % 5;
    resultValues[3] = rand() % 5;
}

void showResult()
{
    int i = 0;

    for(i = 0; i < 4; i++)
    {
        if(resultValues[i] == 0)
            cops->drawBITMAP (&redB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 1)
            cops->drawBITMAP (&yellowB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 2)
            cops->drawBITMAP (&greyB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 3)
            cops->drawBITMAP (&greenB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 4)
            cops->drawBITMAP (&blueB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
    }
}

void resetPieceValues()
{
    memset(&pieceValues, -1, sizeof(pieceValues));
}

void printChecks(int index, int kind)
{
    if(index == 0)
    {
        if(kind == 0)
            cops->drawBITMAP (&whiteB, X_OFFSET, Y_OFFSET + turns*Y_STEP);                       // links oben
        else
            cops->drawBITMAP (&blackB, X_OFFSET, Y_OFFSET + turns*Y_STEP);                       // links oben
    }
    else if(index == 1)
    {
        if(kind == 0)
            cops->drawBITMAP (&whiteB, X_OFFSET, Y_OFFSET + turns*Y_STEP + LENGTH/2);            // links unten
        else
            cops->drawBITMAP (&blackB, X_OFFSET, Y_OFFSET + turns*Y_STEP + LENGTH/2);            // links unten
    }
    else if(index == 2)
    {
        if(kind == 0)
            cops->drawBITMAP (&whiteB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP);            // rechts oben
        else
            cops->drawBITMAP (&blackB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP);            // rechts oben
    }
    else if(index == 3)
    {
        if(kind == 0)
            cops->drawBITMAP (&whiteB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP + LENGTH/2); // rechts unten
        else
            cops->drawBITMAP (&blackB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP + LENGTH/2); // rechts unten
    }
}

int check()
{
    int i = 0;
    int j = 0;
    int found = 0;
    int cntFlags = 0;
    int markSrc[4] = { 0,0,0,0 };
    int markTrg[4] = { 0,0,0,0 };

    // first check if all 4 pieces are set
    for(i = 0; i < 4; i++)
    {
        if(pieceValues[i] == -1)
            return -1;
    }

    // now check the colors which are ok
    for(i = 0; i < 4; i++)
    {
        if(pieceValues[i] == resultValues[i])
        {
            printChecks(cntFlags, 1);
            markSrc[i] = 1;
            markTrg[i] = 1;
            found++;
            cntFlags++;
        }
    }

    // all colors are on the right place ?
    if(found == 4)
        return 1;

    // and then the color which are on the wrong place
    for(i = 0; i < 4; i++)
    {
        if(markSrc[i] == 0)
        {
            for(j = i+1; j < 4; j++)
            {
                if(markTrg[j] == 0)
                {
                    if(pieceValues[i] == resultValues[j])
                    {
                        printChecks(cntFlags, 0);
                        markSrc[i] = 1;
                        markTrg[j] = 1;
                        cntFlags++;
                    }
                }
            }
        }
    }

    return 0;
}

/* init*/
void init(void)
{
    srand(cops->getTick());

    turns = 0;
    pos   = 0;
    piece = 0;

    resetPieceValues();

    pieceValues[0] = piece;

    cops->clearScreen(COLOR_GREEN);

    cops->setFont(STD8X13);
    cops->putS(COLOR_WHITE, COLOR_GREEN, 60,5, "Mastermind by Schoki");
    cops->setFont(STD6X9);


    cops->putS(COLOR_BLACK, COLOR_GREEN, 271,13, "New game");
    cops->putS(COLOR_BLACK, COLOR_GREEN, 295,47, "Quit");
    cops->putS(COLOR_BLACK, COLOR_GREEN, 288,152, "Check");

    initMatrix();

    generate();

    drawPieces();

//    showResult(); // ToDo: muß wieder raus
}

void setCursor(int position, int state)
{
    if(state == 0)
        cops->drawRect(COLOR_WHITE, X_OFFSET + LENGTH + X_OFFSET_BALLS + (position+1)*LENGTH, Y_OFFSET + turns*Y_STEP, 16, 16);
    else
        cops->drawRect(COLOR_RED, X_OFFSET + LENGTH + X_OFFSET_BALLS + (position+1)*LENGTH, Y_OFFSET + turns*Y_STEP, 16, 16);
}

void drawPieces()
{
    char tmp[70];
    if(piece == 0)
    {
        cops->drawBITMAP (&redB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 1)
    {
        cops->drawBITMAP (&yellowB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 2)
    {
        cops->drawBITMAP (&greyB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 3)
    {
        cops->drawBITMAP (&greenB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 4)
    {
        cops->drawBITMAP (&blueB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }

    setCursor(pos, 1);

/*
    cops->fillRect(COLOR_GREEN,4,224,315,12);
    sprintf(tmp, "piece: %d turns: %d pos: %d %d %d %d %d",piece,turns,pos,pieceValues[0],pieceValues[1],pieceValues[2],pieceValues[3]);
    cops->putS(COLOR_WHITE, COLOR_GREEN, 5,225, tmp);*/
}

int eventHandler(int evt)
{
    int ch = -1;

    switch (evt) {
        case BTN_OFF:
        case EVT_QUIT:
                /* get out of here */
                RELEASE(cops)
                break;

                /* move cursor left */
            case BTN_LEFT:

                setCursor(pos, 0);
                pos--;
                if(pos < 0)
                    pos = 3;

                if(pieceValues[pos] == -1)
                {
                    piece = 0;
                    pieceValues[pos] = piece;
                }
                else
                    piece = pieceValues[pos];

                drawPieces();
                break;

                /* move cursor right */
            case BTN_RIGHT:
                setCursor(pos, 0);
                pos++;
                if(pos > 3)
                    pos = 0;

                if(pieceValues[pos] == -1)
                {
                    piece = 0;
                    pieceValues[pos] = piece;
                }
                else
                    piece = pieceValues[pos];

                drawPieces();
                break;

                /* move cursor down */
            case BTN_DOWN:
                piece++;
                if(piece > 4)
                    piece = 0;

                pieceValues[pos] = piece;
                drawPieces();
                break;

                /* move cursor up */
            case BTN_UP:
                piece--;
                if(piece < 0)
                    piece = 4;

                pieceValues[pos] = piece;
                drawPieces();
                break;

            case BTN_F2:
                break;

                /* toggle flag under cursor */
            case BTN_F1:
                ch = check();
                if(ch == 0)
                {
                    setCursor(pos, 0);

                    pos = 0;
                    piece = 0;
                    turns++;

                    if(turns == ROWS-1)
                    {
                        LooseGame();
                    }

                    drawPieces();
                    setCursor(pos, 1);
                }
                else if(ch == 1)
                {
                    WinGame();
                }
                break;

            case BTN_ON: // new game
                cops->clearScreen(COLOR_GREEN);
                init();
                break;

            case BTN_F3: // settings
                break;
    }
}

/* plugin entry point */
int main(int argc,char * * argv)
{
    /* plugin init */
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();

    cops->clearScreen(COLOR_GREEN);
    /* end of plugin init */

	cops->disableMenu();
    cops->setFont(STD6X9);

    init();

    PACK(cops,NULL);

    return 1;
}
