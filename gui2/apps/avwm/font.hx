#include "font.h"

needFont(std6x9);
needFont(std8x13);
needFont(std4x6);
needFont(std4x8);
needFont(std5x7);
needFont(std5x8);
needFont(std6x10);
needFont(std6x12);
needFont(std6x13);
needFont(std6x9);
needFont(std7x13);
needFont(std7x14);
needFont(std8x13);
needFont(cursive);
needFont(dagger);
needFont(inkblot);
needFont(radon);
needFont(radonWide);
needFont(shadow);
needFont(shadowBold);

FONT_ID font_table[NBFONT] ;

FONT_ID plugin_font;

void iniFont(void)
{
    font_table[0]=std6x9;
    font_table[1]=std8x13;
    font_table[2]=std6x9;
    font_table[3]=std8x13;
    font_table[4]=std4x6;
    font_table[5]=std4x8;
    font_table[6]=std5x7;
    font_table[7]=std5x8;
    font_table[8]=std6x10;
    font_table[9]=std6x12;
    font_table[10]=std6x13;
    font_table[11]=std6x9;
    font_table[12]=std7x13;
    font_table[13]=std7x14;
    font_table[14]=std8x13;
    font_table[15]=cursive;
    font_table[16]=dagger;
    font_table[17]=inkblot;
    font_table[18]=radon;
    font_table[19]=radonWide;
    font_table[20]=shadow;
    font_table[21]=shadowBold;
}