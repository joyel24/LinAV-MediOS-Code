/*
* kernel/gfx/gui/widget/virtKbd.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <kernel/kernel.h>
#include <kernel/graphics.h>
#include <kernel/evt.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>
#include <sys_def/string.h>

#define CELL_SPACE        3
#define CELL_DIST         1
#define NB_CELL_PER_LINE  3
#define TOT_WIDTH         320
#define TOT_HEIGHT        240
#define BG_COLOR          COLOR_WHITE
#define TXT_COLOR         COLOR_BLUE
#define SEL_COLOR         COLOR_BLACK
#define BLK_COLOR         COLOR_DARK_GRAY
#define MAX_CHAR_DISP     20

PALETTE pal[2] = {BG_COLOR, TXT_COLOR};

unsigned char leftArrow_DA[13][8] =
    { {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 1, 1, 0, 0, 0, 0},
      {0, 1, 1, 0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
    };

SPRITE leftArrow_SP = {(unsigned int) leftArrow_DA, 8, 13, 1, 1};

unsigned char rightArrow_DA[13][8] =
    { {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 0, 0},
      {0, 0, 0, 0, 0, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 1, 1, 0},
      {0, 0, 0, 0, 1, 1, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
    };

SPRITE rightArrow_SP = {(unsigned int) rightArrow_DA, 8, 13, 1, 1};

char char_horiz[6][NB_CELL_PER_LINE][2]=
{
    /* TOP */
    { "a", "b", "c" },
    { "d", "e", "f" },
    { "g", "h", "i" },
    /* BOTTOM */
    { "", " ", "" },
    { "", "", "" },
    { "", "", "" }
};

char char_vert[6][NB_CELL_PER_LINE][2]=
{
    /* LEFT */
    { "j", "k", "l" },
    { "m", "n", "o" },
    { "p", "q", "r" },
    /* RIGHT */
    { "s", "t", "u" },
    { "v", "w", "x" },
    { "y", "z", "_" }
};

SPRITE * spec_horiz[6][NB_CELL_PER_LINE]=
{
    /* TOP */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    /* BOTTOM */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

SPRITE * spec_vert[6][NB_CELL_PER_LINE]=
{
    /* LEFT */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    /* RIGHT */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

void leftCursor(void);
void rightCursor(void);

void (*action_horiz[6][NB_CELL_PER_LINE])=
{
    /* TOP */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    /* BOTTOM */
    { leftCursor, NULL, rightCursor },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

void (*action_vert[6][NB_CELL_PER_LINE])=
{
    /* TOP */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    /* BOTTOM */
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

int bg_txt_colors[3] = 
{
    COLOR_LIGHT_RED,
    COLOR_LIGHT_YELLOW,
    COLOR_LIGHT_BLUE
};

int coord_horiz[6][2];
int coord_vert[6][2];

int mode=1;
int elem=2;
int cur_index=0;
int str_len=0;
char cur_str[MAX_CHAR_DISP+1];
int txt_x,txt_y;

void calcCoord(void)
{
    int h=0,w=0,x,y;
    gfx_getStringSize("M", &w, &h);
    /* horizontal lines coordinates */
    x=(TOT_WIDTH-(NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST))/2;
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
    
    y=(TOT_HEIGHT-(NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST))/2;    
    coord_vert[0][1]=coord_vert[1][1]=coord_vert[2][1]=coord_vert[3][1]=coord_vert[4][1]=coord_vert[5][1]=y;
    
    txt_x=(TOT_WIDTH-w*MAX_CHAR_DISP)/2;
    txt_y=(TOT_HEIGHT-h)/2;
}

void drawHoriz(int pos)
{
    int h=0,w=0,i;
    gfx_getStringSize("M", &w, &h);
    for(i=0;i<NB_CELL_PER_LINE;i++)
    {
        if(char_horiz[pos][i][0] || spec_horiz[pos][i])
            gfx_fillRect(bg_txt_colors[i],coord_horiz[pos][0]+i*(w+CELL_SPACE),coord_horiz[pos][1],w+2*CELL_DIST,h+2*CELL_DIST);
        if(char_horiz[pos][i][0])
            gfx_putS(TXT_COLOR,bg_txt_colors[i],coord_horiz[pos][0]+i*(w+CELL_SPACE)+CELL_DIST,
                    coord_horiz[pos][1]+CELL_DIST,char_horiz[pos][i]);
        else
            if(spec_horiz[pos][i])
            {
                pal[0]=bg_txt_colors[i];
                gfx_drawSprite ((PALETTE*)&pal, spec_horiz[pos][i], coord_horiz[pos][0]+i*(w+CELL_SPACE)+CELL_DIST,
                        coord_horiz[pos][1]+CELL_DIST);
            }
    }
}

void drawVert(int pos)
{
    int h=0,w=0,i;
    gfx_getStringSize("M", &w, &h);
    for(i=0;i<NB_CELL_PER_LINE;i++)
    {
        if(char_vert[pos][i][0] || spec_vert[pos][i])
            gfx_fillRect(bg_txt_colors[i],coord_vert[pos][0],coord_vert[pos][1]+i*(h+CELL_SPACE),w+2*CELL_DIST,h+2*CELL_DIST);
        if(char_vert[pos][i][0])
            gfx_putS(TXT_COLOR,bg_txt_colors[i],coord_vert[pos][0]+CELL_DIST,
                    coord_vert[pos][1]+i*(h+CELL_SPACE)+CELL_DIST,char_vert[pos][i]);
        else
            if(spec_vert[pos][i])
            {
                pal[0]=bg_txt_colors[i];
                gfx_drawSprite ((PALETTE*)&pal, spec_vert[pos][i],coord_vert[pos][0]+i*(w+CELL_SPACE)+CELL_DIST,
                        coord_vert[pos][1]+CELL_DIST);
            }
    }
}

void drawTxtZone(void)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    gfx_drawRect(BLK_COLOR,txt_x-2*CELL_DIST,txt_y-2*CELL_DIST,MAX_CHAR_DISP*w+4*CELL_DIST,h+4*CELL_DIST+2);
    /* +2 => space for the cursor*/
}

void drawBtn(void)
{    
    int x,y;
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    x=coord_horiz[0][0]+NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST;
    y=coord_vert[0][1]+NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST;
    x=x+(TOT_WIDTH-x)/2;
    y=y+(TOT_HEIGHT-y)/2;
    gfx_getStringSize("F3", &w, &h);
    gfx_drawRect(BLK_COLOR,x-w-CELL_SPACE-2*CELL_DIST,y-h-CELL_SPACE-2*CELL_DIST,
            3*(w+CELL_SPACE)-CELL_SPACE+4*CELL_DIST,2*(h+CELL_SPACE)-CELL_SPACE+4*CELL_DIST);
    gfx_putS(TXT_COLOR,bg_txt_colors[0],x-w-CELL_SPACE,y,"F1");
    gfx_putS(TXT_COLOR,bg_txt_colors[1],x,y-h-CELL_SPACE,"F2");
    gfx_putS(TXT_COLOR,bg_txt_colors[2],x+w+CELL_SPACE,y,"F3"); 
}

void drawPage(void)
{
    int i;
    gfx_clearScreen(BG_COLOR);
    for(i=0;i<6;i++)
    {
        drawVert(i);
        drawHoriz(i);
    }
}

void gfx_putSelect(int color)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    printk("[putCursor] %s %s %d\n",color==COLOR_WHITE?"WHITE":"BLUE",mode==1?"HORIZ":"VERT",elem);
    if(mode) // horizontal
    {
        gfx_drawRect(color,coord_horiz[elem][0],coord_horiz[elem][1],NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST,
                        h+2*CELL_DIST);
    }
    else    // vertical
    {
        gfx_drawRect(color,coord_vert[elem][0],coord_vert[elem][1],w+2*CELL_DIST,
                        NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST);
    }
}

void putCursor(int color)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    gfx_drawLine(color,txt_x+cur_index*w,txt_y+h+1,txt_x+cur_index*w+w,txt_y+h+1);
}

void leftCursor(void)
{
    if(cur_index>0)
    {
        putCursor(BG_COLOR);
        cur_index--;
        putCursor(SEL_COLOR);
    }
}

void rightCursor(void)
{
    if(cur_index<str_len)
    {
        putCursor(BG_COLOR);
        cur_index++;
        putCursor(SEL_COLOR);
    }
}

void virtKbdEvtHandler(int evt_hanlder)
{
    int h=0,w=0;
    int stopLoop=0;
    int char_num=0;
    char * str;
    void (*routine)(void);
    gfx_getStringSize("M", &w, &h);
    char evt=0;
    
    while(!stopLoop)
    {
        evt = evt_getHandler(evt_hanlder);
        if(!evt)
            continue;
        char_num=0;    
        switch (evt) {        
            case BTN_DOWN:
                if(mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==5)
                        elem=0;
                    else
                        elem++;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=3;
                    mode=1;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_UP:
                if(mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==0)
                        elem=5;
                    else
                        elem--;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=2;
                    mode=1;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_LEFT:
                if(!mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==0)
                        elem=5;
                    else
                        elem--;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=2;
                    mode=0;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_RIGHT:
                if(!mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==5)
                        elem=0;
                    else
                        elem++;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=3;
                    mode=0;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_OFF:
                if(str_len+1==MAX_CHAR_DISP)
                    str_len++;
                cur_str[str_len]='\0';
                /* get out of here */            
                stopLoop=1;
                break;        
            case BTN_F3:
                char_num++;
            case BTN_F2:
                char_num++;
            case BTN_F1:
                if(cur_index<MAX_CHAR_DISP)
                {
                    str=mode==1?char_horiz[elem][char_num]:char_vert[elem][char_num];
                    if(str[0])
                    {
                        putCursor(BG_COLOR);
                        gfx_putS(TXT_COLOR,BG_COLOR,txt_x+cur_index*w,txt_y,str);
                        cur_str[cur_index]=str[0];
                        if(cur_index+1!=MAX_CHAR_DISP)
                        {
                            if(cur_index==str_len)
                                str_len++;
                            cur_index++;
                        }
                        putCursor(SEL_COLOR);
                    }
                    else
                    {
                        if(mode==1)
                        {
                            if(action_horiz[elem][char_num])
                            {
                                routine=action_horiz[elem][char_num];
                                routine();
                            }
                        }
                        else
                        {
                            if(action_vert[elem][char_num])
                            {
                                routine=action_vert[elem][char_num];
                                routine();
                            }
                        }
                    }
                }
                break;
        }
    }    
}

void iniSprite(void)
{
    spec_horiz[3][0]=&leftArrow_SP;
    spec_horiz[3][2]=&rightArrow_SP;
}

void virtKbd(int evt_hanlder)
{
    gfx_fontSet(STD8X13);
    calcCoord();
    iniSprite();
    drawPage();
    drawBtn();
    drawTxtZone();
    gfx_putSelect(SEL_COLOR);  
    putCursor(SEL_COLOR);
    virtKbdEvtHandler(evt_hanlder);
}
