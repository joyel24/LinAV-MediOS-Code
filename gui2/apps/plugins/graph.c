/*
* graph.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Zakk Roberts
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

#define MODE_INPUT 1
#define MODE_GRAPH 2

#define OPER_MULTIPLY 1
#define OPER_EXPONENT 2
#define OPER_DIVIDE   3
#define OPER_SUBTRACT 4
#define OPER_ADD      5

/********************
 * EQUATION VARIABLES
 *******************/
int x=2;
int y_int=0;
int oper1 = OPER_MULTIPLY;
int oper2 = OPER_ADD;

/*****************
 * OTHER VARIABLES
 ****************/
int cursorpos = 1;
int mode = MODE_INPUT;
int xscale = 1;
int yscale = 1;

/* Initialization, REQUIRED */
struct client_operations * cops;

/*******************
 * Draw text on side
 ******************/
void drawFuncBar(void)
{
    int w, h;

    cops->setFont(STD6X10);

    cops->getStringS("Graph", &w, &h);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 320-w-5,15, "Graph");

    cops->getStringS("Quit", &w, &h);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 320-w-5,45, "Quit");

    cops->setFont(STD8X13);
}

void drawCursor(void)
{
    if(cursorpos == 1)
        cops->drawLine(COLOR_RED, 58, 65, 64, 65);
    else if(cursorpos == 2)
        cops->drawLine(COLOR_RED, 74, 65, 89, 65);
    else if(cursorpos == 3)
        cops->drawLine(COLOR_RED, 98, 65, 104, 65);
    else if(cursorpos == 4)
        cops->drawLine(COLOR_RED, 114, 65, 129, 65);
}

int power(int num)
{
    int i, j;

    if(num < 0)
        num = -num;

    j=num;

    for (i=1; i<x; i++)
        num *= j;

    return num;
}

/****************
 * Graph equation
 ***************/
void graph(void)
{
    int i;
    int lasti;

    for(i=0-LCD_WIDTH/2, lasti=i-1; i<LCD_WIDTH/2; i++)
    {
        if(oper2 == OPER_ADD)
        {
            if(oper1 == OPER_MULTIPLY)
                cops->drawPixel(COLOR_WHITE, LCD_WIDTH/2 + i*yscale, (LCD_HEIGHT/2)-(x*i)-y_int);
            else if(oper1 == OPER_EXPONENT)
                cops->drawPixel(COLOR_WHITE, LCD_WIDTH/2 + i*yscale, (LCD_HEIGHT/2)-power(i)-y_int);
            else if(oper1 == OPER_DIVIDE)
                cops->drawPixel(COLOR_WHITE, LCD_WIDTH/2 + i*yscale, (LCD_HEIGHT/2)-(i/x)-y_int);
        }
        else if(oper2 == OPER_SUBTRACT)
        {
            if(oper1 == OPER_MULTIPLY)
                cops->drawPixel(COLOR_WHITE, LCD_WIDTH/2 + i*yscale, (LCD_HEIGHT/2)-(x*i)+y_int);
            else if(oper1 == OPER_EXPONENT)
                cops->drawPixel(COLOR_WHITE, LCD_WIDTH/2 + i*yscale, (LCD_HEIGHT/2)-power(i)+y_int);
            else if(oper1 == OPER_DIVIDE)
                cops->drawPixel(COLOR_WHITE, LCD_WIDTH/2 + i*yscale, (LCD_HEIGHT/2)-(i/x)+y_int);
        }
    }
}

/***************
 * Main function
 **************/
void input(void)
{
    char graphtext[50];
    char graph_x[6];
    char graph_y_int[5];

    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 10, "Graph plugin by midk");

    if(oper1 == OPER_MULTIPLY)
        sprintf(graph_x, "X * %02d", x);
    else if(oper1 == OPER_EXPONENT)
        sprintf(graph_x, "X ^ %02d", x);
    else if(oper1 == OPER_DIVIDE)
        sprintf(graph_x, "X / %02d", x);

    if(oper2 == OPER_ADD)
        sprintf(graph_y_int, "+ %02d", y_int);
    else if(oper2 == OPER_SUBTRACT)
        sprintf(graph_y_int, "- %02d", y_int);

    sprintf(graphtext, "Y = %s %s", graph_x, graph_y_int);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 50, graphtext);

    drawCursor();
}

/***************
 * Handle events
 **************/
int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            if(mode == MODE_GRAPH)
            {
                cops->clearScreen(COLOR_BLACK);
                mode = MODE_INPUT;
                drawFuncBar();
            }
            else if(mode == MODE_INPUT)
                RELEASE(cops); /* we're done */
            break;

        case BTN_ON:
            if(mode == MODE_INPUT)
            {
                cops->clearScreen(COLOR_BLACK);
                cops->drawLine(COLOR_DARK_GRAY, 0, LCD_HEIGHT/2, 320, LCD_HEIGHT/2);
                cops->drawLine(COLOR_DARK_GRAY, LCD_WIDTH/2, 0, LCD_WIDTH/2, 240);
                mode = MODE_GRAPH;
            }
            break;

        case BTN_RIGHT:
            if(mode == MODE_INPUT)
            {
                switch(cursorpos)
                {
                    case 1:
                        cursorpos = 2;
                        cops->drawLine(COLOR_BLACK, 58, 65, 65, 65);
                        break;

                    case 2:
                        cursorpos = 3;
                        cops->drawLine(COLOR_BLACK, 74, 65, 89, 65);
                        break;

                    case 3:
                        cursorpos = 4;
                        cops->drawLine(COLOR_BLACK, 98, 65, 105, 65);
                        break;
                }
            }
            break;

        case BTN_LEFT:
            if(mode == MODE_INPUT)
            {
                switch(cursorpos)
                {
                    case 4:
                        cursorpos = 3;
                        cops->drawLine(COLOR_BLACK, 114, 65, 129, 65);
                        break;

                    case 3:
                        cursorpos = 2;
                        cops->drawLine(COLOR_BLACK, 98, 65, 105, 65);
                        break;

                    case 2:
                        cursorpos = 1;
                        cops->drawLine(COLOR_BLACK, 74, 65, 89, 65);
                        break;
                }
            }
            break;

        case BTN_UP:
            if(mode == MODE_INPUT)
            {
                switch(cursorpos)
                {
                    case 1:
                        if(oper1 < 3)
                            oper1++;
                        break;

                    case 2:
                        if(x < 10)
                            x++;
                        break;

                    case 3:
                        if(oper2 != OPER_ADD)
                            oper2 = OPER_ADD;
                        break;

                    case 4:
                        if(y_int < 50)
                            y_int++;
                        break;
                    }
                }
                break;

        case BTN_DOWN:
            if(mode == MODE_INPUT)
            {
                switch(cursorpos)
                {
                    case 1:
                        if(oper1 > 1)
                            oper1--;
                        break;

                    case 2:
                        if(x > 1)
                            x--;
                        break;

                    case 3:
                        if(oper2 != OPER_SUBTRACT)
                            oper2 = OPER_SUBTRACT;
                        break;

                    case 4:
                        if(y_int > 0)
                            y_int--;
                        break;
                    }
                }
                break;
    }
    return 1;
}

void mainloop(void)
{
    if(mode == MODE_INPUT)
        input();
    else if(mode == MODE_GRAPH)
        graph();
}

int main(int argc,char * * argv)
{
    /* Initialization, REQUIRED */
    REGISTER(cops,eventHandler,0);

    cops->hideSBar(); /* shut off the status bar */
    cops->disableMenu(); /* disable the F3=MENU function */
    cops->clearScreen(COLOR_BLACK); /* clear the LCD to black */
    cops->setFont(STD8X13);
    drawFuncBar();

    /* These two functions REQUIRED */
    /* Usage: PACK(cops, <main function>)*/
    PACK(cops, mainloop);

    return 1;
}
