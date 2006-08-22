/*
*   mastermind.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* initial code from Schoki - Linav project 2004
* 
*
*/

#include <api.h>
#include <evt.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

// upper left corner of drawing zone (X_OFFSET,Y_OFFSET)
// drawing zone = result zone + balls zone
#define X_OFFSET 80           
#define Y_OFFSET 30

// X space between end of result zone and begining of balls zone 
#define X_OFFSET_BALLS 10

// width of the result zone & of each ball (also internal height of result zone)
#define LENGTH 16

// number of rows
#define ROWS 8

// height of a row
#define Y_STEP 18

/***************************************
*
* Warning some coordinate are still hardcoded: for menu, and other texts
*
****************************************/

#define MAX_COLORS 8


void drawPieces();
void showResult();
void setCursor(int position, int state);
void PrintCntColors();

/* the cursor coordinates */
int turns = 0;
int pos = 0;
int piece = 0;
int cntColors = 5;

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

unsigned char black[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 0, 0,15,15,15,15,15,15,15},
      {15,15,15,15,15,15, 0, 0, 0, 0,15,15,15,15,15,15},
      {15,15,15,15,15, 0, 0, 0, 0, 0, 0,15,15,15,15,15},
      {15,15,15,15, 0, 0, 0, 0, 0, 0, 0, 0,15,15,15,15},
      {15,15,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,15,15,15},
      {15,15,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,15,15,15},
      {15,15,15,15, 0, 0, 0, 0, 0, 0, 0, 0,15,15,15,15},
      {15,15,15,15,15, 0, 0, 0, 0, 0, 0,15,15,15,15,15},
      {15,15,15,15,15,15, 0, 0, 0, 0,15,15,15,15,15,15},
      {15,15,15,15,15,15,15, 0, 0,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char purple[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,43,43,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,43,43,43,43,15,15,15,15,15,15},
      {15,15,15,15,15,43,43,43,43,43,43,15,15,15,15,15},
      {15,15,15,15,43,43,43,43,43,43,43,43,15,15,15,15},
      {15,15,15,43,43,43,43,43,43,43,43,43,43,15,15,15},
      {15,15,15,43,43,43,43,43,43,43,43,43,43,15,15,15},
      {15,15,15,15,43,43,43,43,43,43,43,43,15,15,15,15},
      {15,15,15,15,15,43,43,43,43,43,43,15,15,15,15,15},
      {15,15,15,15,15,15,43,43,43,43,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,43,43,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char orange[16][16] =
    { {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,182,182,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,182,182,182,182,15,15,15,15,15,15},
      {15,15,15,15,15,182,182,182,182,182,182,15,15,15,15,15},
      {15,15,15,15,182,182,182,182,182,182,182,182,15,15,15,15},
      {15,15,15,182,182,182,182,182,182,182,182,182,182,15,15,15},
      {15,15,15,182,182,182,182,182,182,182,182,182,182,15,15,15},
      {15,15,15,15,182,182,182,182,182,182,182,182,15,15,15,15},
      {15,15,15,15,15,182,182,182,182,182,182,15,15,15,15,15},
      {15,15,15,15,15,15,182,182,182,182,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,182,182,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
    };

unsigned char whitePin[8][8] =
    { {15,15,15,15,15,15,15,15},
      {15,00,00,00,00,00,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,15,15,15,15,00,15},
      {15,00,00,00,00,00,00,15},
      {15,15,15,15,15,15,15,15}
    };

unsigned char blackPin[8][8] =
    { {15,15,15,15,15,15,15,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,00,00,00,00,00,00,15},
      {15,15,15,15,15,15,15,15}
    };

unsigned char emptyPin[8][8] =
    { {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,00,00,15,15,15},
      {15,15,15,00,00,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15},
      {15,15,15,15,15,15,15,15}
    };

static BITMAP  clearB  =  {(unsigned int)    clear,  16, 16, 0, 0};
static BITMAP    redB  =  {(unsigned int)      red,  16, 16, 0, 0};
static BITMAP yellowB  =  {(unsigned int)   yellow,  16, 16, 0, 0};
static BITMAP   greyB  =  {(unsigned int)     grey,  16, 16, 0, 0};
static BITMAP  greenB  =  {(unsigned int)    green,  16, 16, 0, 0};
static BITMAP   blueB  =  {(unsigned int)     blue,  16, 16, 0, 0};
static BITMAP  blackB  =  {(unsigned int)    black,  16, 16, 0, 0};
static BITMAP  purpleB =  {(unsigned int)   purple,  16, 16, 0, 0};
static BITMAP  orangeB =  {(unsigned int)   orange,  16, 16, 0, 0};

static BITMAP  blackPinB =  {(unsigned int)  blackPin,   8,  8, 0, 0};
static BITMAP  whitePinB =  {(unsigned int)  whitePin,   8,  8, 0, 0};
static BITMAP  emptyPinB =  {(unsigned int)  emptyPin,   8,  8, 0, 0};

void delay(int value)
{
    int i,j,k;

    for(i=0; i<value;i++)
        for(j=0; j<value;j++)
            for(k=0; k<value;k++){}
}

void WinGame()
{
    showResult();

    gfx_fontSet(STD8X13);
    gfx_putS(COLOR_BLACK, COLOR_WHITE, 70,90, "!!! Y O U  W I N !!!");
    gfx_fontSet(STD6X9);
}

void LooseGame()
{
    showResult();

    gfx_fontSet(STD8X13);
    gfx_putS(COLOR_BLACK, COLOR_WHITE, 70,90, "!!! Y O U  L O O S E !!!");
    gfx_fontSet(STD6X9);
}

void initMatrix()
{
    int i = 0;

    for(i = 0; i < ROWS; i++)
    {
        // Evaluation zone
        gfx_drawBitmap (&emptyPinB, X_OFFSET, Y_OFFSET + i*Y_STEP);                       // upper left
        gfx_drawBitmap (&emptyPinB, X_OFFSET, Y_OFFSET + i*Y_STEP + LENGTH/2);            // lower left
        gfx_drawBitmap (&emptyPinB, X_OFFSET + LENGTH/2, Y_OFFSET + i*Y_STEP);            // upper right
        gfx_drawBitmap (&emptyPinB, X_OFFSET + LENGTH/2, Y_OFFSET + i*Y_STEP + LENGTH/2); // lower right

        // Black balls
        gfx_drawBitmap (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS +   LENGTH, Y_OFFSET + i*Y_STEP);
        gfx_drawBitmap (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS + 2*LENGTH, Y_OFFSET + i*Y_STEP);
        gfx_drawBitmap (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS + 3*LENGTH, Y_OFFSET + i*Y_STEP);
        gfx_drawBitmap (&clearB, X_OFFSET + LENGTH + X_OFFSET_BALLS + 4*LENGTH, Y_OFFSET + i*Y_STEP);
    }
}

void generate()
{
    resultValues[0] = rand() % cntColors;
    resultValues[1] = rand() % cntColors;
    resultValues[2] = rand() % cntColors;
    resultValues[3] = rand() % cntColors;
}

void showResult()
{
    int i = 0;

    for(i = 0; i < 4; i++)
    {
        if(resultValues[i] == 0)
            gfx_drawBitmap (&redB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 1)
            gfx_drawBitmap (&yellowB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 2)
            gfx_drawBitmap (&greyB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 3)
            gfx_drawBitmap (&greenB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 4)
            gfx_drawBitmap (&blueB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 5)
            gfx_drawBitmap (&blackB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 6)
            gfx_drawBitmap (&purpleB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
        else if(resultValues[i] == 7)
            gfx_drawBitmap (&orangeB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (i+1)*LENGTH, Y_OFFSET + (ROWS+1)*Y_STEP);
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
            gfx_drawBitmap (&whitePinB, X_OFFSET, Y_OFFSET + turns*Y_STEP);                       // upper left
        else
            gfx_drawBitmap (&blackPinB, X_OFFSET, Y_OFFSET + turns*Y_STEP);                       // upper left
    }
    else if(index == 1)
    {
        if(kind == 0)
            gfx_drawBitmap (&whitePinB, X_OFFSET, Y_OFFSET + turns*Y_STEP + LENGTH/2);            // lower left
        else
            gfx_drawBitmap (&blackPinB, X_OFFSET, Y_OFFSET + turns*Y_STEP + LENGTH/2);            // lower left
    }
    else if(index == 2)
    {
        if(kind == 0)
            gfx_drawBitmap (&whitePinB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP);            // upper right
        else
            gfx_drawBitmap (&blackPinB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP);            // upper right
    }
    else if(index == 3)
    {
        if(kind == 0)
            gfx_drawBitmap (&whitePinB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP + LENGTH/2); // lower right
        else
            gfx_drawBitmap (&blackPinB, X_OFFSET + LENGTH/2, Y_OFFSET + turns*Y_STEP + LENGTH/2); // lower right
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
            for(j = 0; j < 4; j++)
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
    srand(tmr_getTick());

    turns = 0;
    pos   = 0;
    piece = 0;

    resetPieceValues();

    pieceValues[0] = piece;

    gfx_clearScreen(COLOR_GREEN);

    gfx_fontSet(STD8X13);
    gfx_putS(COLOR_WHITE, COLOR_GREEN, 60,5, "Mastermind by Schoki");
    gfx_fontSet(STD6X9);

    gfx_putS(COLOR_BLACK, COLOR_GREEN, 271,13, "New game");
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 295,47, "Quit");
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 290,152, "Check");

    PrintCntColors();

    gfx_putS(COLOR_WHITE, COLOR_GREEN, 15,220, "black=right color and position  white=right color");

    initMatrix();

    generate();

    drawPieces();

    //showResult();
}

void PrintCntColors()
{
    char tmp[20];
    sprintf(tmp,"%d Colors", cntColors);
    gfx_putS(COLOR_BLACK, COLOR_GREEN, 267,180, tmp);
}

void setCursor(int position, int state)
{
    if(state == 0)
        gfx_drawRect(COLOR_WHITE, X_OFFSET + LENGTH + X_OFFSET_BALLS + (position+1)*LENGTH, Y_OFFSET + turns*Y_STEP, 16, 16);
    else
        gfx_drawRect(COLOR_RED, X_OFFSET + LENGTH + X_OFFSET_BALLS + (position+1)*LENGTH, Y_OFFSET + turns*Y_STEP, 16, 16);
}

void drawPieces()
{
    if(piece == 0)
    {
        gfx_drawBitmap (&redB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 1)
    {
        gfx_drawBitmap (&yellowB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 2)
    {
        gfx_drawBitmap (&greyB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 3)
    {
        gfx_drawBitmap (&greenB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 4)
    {
        gfx_drawBitmap (&blueB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    }
    else if(piece == 5)
        gfx_drawBitmap (&blackB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    else if(piece == 6)
        gfx_drawBitmap (&purpleB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);
    else if(piece == 7)
        gfx_drawBitmap (&orangeB, X_OFFSET + LENGTH + X_OFFSET_BALLS + (pos+1)*LENGTH, Y_OFFSET + turns*Y_STEP);

    setCursor(pos, 1);

/*
    fillRect(COLOR_GREEN,4,224,315,12);
    sprintf(tmp, "piece: %d turns: %d pos: %d %d %d %d %d",piece,turns,pos,pieceValues[0],pieceValues[1],pieceValues[2],pieceValues[3]);
    gfx_putS(COLOR_WHITE, COLOR_GREEN, 5,225, tmp);*/
}

void eventHandlerLoop(void)
{
    int ch = -1;
	int stop=0;
	int end=0;
	int evt;
	int evt_handler=evt_getHandler(BTN_CLASS);
    while(!stop)
    {
		evt=evt_getStatus(evt_handler);
		if(evt==NO_EVENT)
			continue;
		if(!end)
		{
			switch (evt) {
				case BTN_OFF:
                    /* get out of here */
					stop=1;
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
                    if(piece > cntColors)
                        piece = 0;

                    pieceValues[pos] = piece;
                    drawPieces();
                    break;

                    /* move cursor up */
                case BTN_UP:
                    piece--;
                    if(piece < 0)
                        piece = cntColors;

                    pieceValues[pos] = piece;
                    drawPieces();
                    break;

                case BTN_F3:
                    break;

                    /* toggle flag under cursor */
                case BTN_F1:
                    ch = check();
                    if(ch == 0)
                    {
                        resetPieceValues();

                        setCursor(pos, 0);

                        pos = 0;
                        piece = 0;
                        turns++;

                        if(turns == ROWS)
                        {
                            LooseGame();
							end=1;
                        }
                        else
                        {
                            pieceValues[0] = piece;
                            drawPieces();
                            setCursor(pos, 1);
                        }
                    }
                    else if(ch == 1)
                    {
                        WinGame();
						end=1;
                    }
                    break;

                case BTN_ON: // new game
                    gfx_clearScreen(COLOR_GREEN);
                    init();
                    break;

                case BTN_F2: // change amount of colors

                    if(cntColors > MAX_COLORS)
                        cntColors = 4;
                    else
                        cntColors++;

                    // restart
                    gfx_clearScreen(COLOR_GREEN);
                    init();
                    break;
        }
		}
		else
		{
			switch (evt) {
				case BTN_OFF:
                    /* get out of here */
					stop=1;
                    break; 
				case BTN_ON: // new game
                    gfx_clearScreen(COLOR_GREEN);
                    init();
					end=0;
                    break;
			}
		}
    }
	evt_freeHandler(evt_handler);
}

/* plugin entry point */
void app_main(int argc,char * * argv)
{
	gfx_openGraphics();

    gfx_clearScreen(COLOR_GREEN);
    /* end of plugin init */

    gfx_fontSet(STD6X9);

    init();

	eventHandlerLoop();
}

