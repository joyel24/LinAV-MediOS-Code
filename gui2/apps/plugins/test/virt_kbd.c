#include <stdlib.h>
#include <stdio.h>

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"
#include "font.h"

#define CELL_SPACE        3
#define CELL_DIST         0
#define NB_CELL_PER_LINE  3
#define TOT_WIDTH         320
#define TOT_HEIGHT        240
#define BG_COLOR          COLOR_WHITE
#define TXT_COLOR         COLOR_BLUE
#define SEL_COLOR         COLOR_RED
#define MAX_CHAR          15

struct client_operations * cops;

char char_horiz[6][NB_CELL_PER_LINE][2]=
{
    /* TOP */
    { "A", "B", "C" },
    { "D", "E", "F" },
    { "G", "H", "I" },
    /* BOTTOM */
    { " ", " ", " " },
    { " ", " ", " " },
    { " ", " ", " " }
};

char char_vert[6][NB_CELL_PER_LINE][2]=
{
    /* LEFT */
    { "J", "K", "L" },
    { "M", "N", "O" },
    { "P", "Q", "R" },
    /* RIGHT */
    { "S", "T", "U" },
    { "V", "W", "X" },
    { "Y", "Z", " " }
};

int coord_horiz[6][2];
int coord_vert[6][2];

int mode=1;
int elem=2;
int index=0;

int txt_x,txt_y;

void calcCoord(void)
{
    int h=0,w=0,x,y;
    cops->getStringS("M", &w, &h);
    /* horizontal lines coordinates */
    x=(TOT_WIDTH-(NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE))/2;
    coord_horiz[0][0]=coord_horiz[1][0]=coord_horiz[2][0]=coord_horiz[3][0]=coord_horiz[4][0]=coord_horiz[5][0]=x;
    
    coord_horiz[0][1]=CELL_DIST+1;
    coord_horiz[1][1]=coord_horiz[0][1]+h+CELL_SPACE;
    coord_horiz[2][1]=coord_horiz[1][1]+h+CELL_SPACE;
    
    coord_horiz[5][1]=TOT_HEIGHT-1-CELL_DIST-h;
    coord_horiz[4][1]=coord_horiz[5][1]-CELL_SPACE-h;
    coord_horiz[3][1]=coord_horiz[4][1]-CELL_SPACE-h;
//     
    /* vertical lines coordinates */
    coord_vert[0][0]=CELL_DIST+1;
    coord_vert[1][0]=coord_vert[0][0]+w+CELL_SPACE;
    coord_vert[2][0]=coord_vert[1][0]+w+CELL_SPACE;
    
    coord_vert[5][0]=TOT_WIDTH-1-CELL_DIST-w;
    coord_vert[4][0]=coord_vert[5][0]-CELL_SPACE-w;
    coord_vert[3][0]=coord_vert[4][0]-CELL_SPACE-w;
    
    y=(TOT_HEIGHT-(NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE))/2;    
    coord_vert[0][1]=coord_vert[1][1]=coord_vert[2][1]=coord_vert[3][1]=coord_vert[4][1]=coord_vert[5][1]=y;
    
    txt_x=(TOT_WIDTH-h*MAX_CHAR)/2;
    txt_y=(TOT_HEIGHT-h)/2;
}

void drawHoriz(int pos)
{
    int h=0,w=0,i;
    cops->getStringS("M", &w, &h);
    for(i=0;i<NB_CELL_PER_LINE;i++)
        cops->putS(TXT_COLOR,BG_COLOR,coord_horiz[pos][0]+i*(w+CELL_SPACE),coord_horiz[pos][1],char_horiz[pos][i]);
}

void drawVert(int pos)
{
    int h=0,w=0,i;
    cops->getStringS("M", &w, &h);
    for(i=0;i<NB_CELL_PER_LINE;i++)
        cops->putS(TXT_COLOR,BG_COLOR,coord_vert[pos][0],coord_vert[pos][1]+i*(h+CELL_SPACE),char_vert[pos][i]);
}

void drawPage(int page)
{
    int i;
    cops->clearScreen(BG_COLOR);
    for(i=0;i<6;i++)
    {
        drawVert(i);
        drawHoriz(i);
    }
}

void putCursor(int color)
{
    int h=0,w=0;
    cops->getStringS("M", &w, &h);
    //printf("[putCursor] %s %s %d\n",color==COLOR_WHITE?"WHITE":"BLUE",mode==1?"HORIZ":"VERT",elem);
    if(mode) // horizontal
    {
        cops->drawRect(color,coord_horiz[elem][0],coord_horiz[elem][1],NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE,h);
    }
    else    // vertical
    {
        cops->drawRect(color,coord_vert[elem][0],coord_vert[elem][1],w,NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE);
    }
}

int eventHandler(int evt)
{
    int h=0,w=0;
    int char_num=0;
    cops->getStringS("M", &w, &h);
    switch (evt) {
        case BTN_DOWN:
            if(mode)
            {
                putCursor(COLOR_WHITE);
                if(elem==5)
                    elem=0;
                else
                    elem++;
                putCursor(COLOR_BLUE);    
            }
            else
            {
                putCursor(COLOR_WHITE);
                elem=3;
                mode=1;
                putCursor(COLOR_BLUE);
            }
            break;
        case BTN_UP:
            if(mode)
            {
                putCursor(COLOR_WHITE);
                if(elem==0)
                    elem=5;
                else
                    elem--;
                putCursor(COLOR_BLUE);    
            }
            else
            {
                putCursor(COLOR_WHITE);
                elem=2;
                mode=1;
                putCursor(COLOR_BLUE);
            }
            break;
        case BTN_LEFT:
            if(!mode)
            {
                putCursor(COLOR_WHITE);
                if(elem==0)
                    elem=5;
                else
                    elem--;
                putCursor(COLOR_BLUE);    
            }
            else
            {
                putCursor(COLOR_WHITE);
                elem=2;
                mode=0;
                putCursor(COLOR_BLUE);
            }
            break;
        case BTN_RIGHT:
            if(!mode)
            {
                putCursor(COLOR_WHITE);
                if(elem==5)
                    elem=0;
                else
                    elem++;
                putCursor(COLOR_BLUE);    
            }
            else
            {
                putCursor(COLOR_WHITE);
                elem=3;
                mode=0;
                putCursor(COLOR_BLUE);
            }
            break;
        case BTN_OFF:
        case EVT_QUIT:
            /* get out of here */
                RELEASE(cops)
            break;        
        case BTN_F3:
            char_num++;
        case BTN_F2:
            char_num++;
        case BTN_F1:
            if(index<MAX_CHAR)
            {
                cops->putS(TXT_COLOR,BG_COLOR,txt_x+index*w,txt_y,
                    mode==1?char_horiz[elem][char_num]:char_vert[elem][char_num]);
                index++;
            }
            break;
    }
    return 1;
}


int main(int argc,char ** argv)
{
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();  
    cops->disableMenu();
    cops->setFont(STD8X13);
    calcCoord();
    drawPage(0); 
    putCursor(COLOR_BLUE);    
    PACK(cops,NULL)
    return 1;
}
