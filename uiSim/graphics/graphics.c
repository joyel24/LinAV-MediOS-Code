/*
* graphics.c
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
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
** declaration des variables utilisées par Xlib 
*/

  char* displayName = 0;			/* Nom du serveur */
  Display* display;				/* display */
  int screen; 					/* numero de l'ecran */
  Window window;				/* creation de la fenetre */
  GC gc;					/* contexte graphique */
  XEvent event;					
  char texte[] = "LinAV project";		/*pour les evenements */

int ini_graphics()
{
 /*connexion au serveur X */
 
 display = XOpenDisplay(displayName);
 
 if(!display) 
 {
	printf("connexion impossible au serveur X");
	exit(1);
 }
	      
 /*recuperation du contexte graphique (infos grpahiques) */

 screen = DefaultScreen(display);
 gc = DefaultGC(display, screen);

 /*creation de la fenetre window*/

 window = XCreateSimpleWindow(
	  display, 	        	   // Display
	  DefaultRootWindow(display),  // Fenêtre mère
	  0, 0, 300, 100, 	   // Géométrie 
	  10,			   // Largeur du bord
	  BlackPixel(display, screen), // Couleur du bord
	  WhitePixel(display, screen)  // Couleur du fond
	  );
	  
 if(!window) 
 {
	printf("impossible de creer la fenetre");
	exit(1);
 }
 
  /* Mise en place du titre */
  
  XStoreName(display, window, texte);

  /* Sélection des événements */
  
  XSelectInput(display, window, ExposureMask);

  /* Affichage de la fenêtre */
  
  XMapWindow(display, window);
	    
 /* Boucle d'attente d'événements */
  
  while(1) 
  {
    /* Récupération de l'événement suivant*/
    XNextEvent(display, &event);
    
    switch (event.type) 
    {
      case Expose : 
       printf("Événement Expose :)\n");
      break;
    }
  }
  return 0;

}

void close_graphics()
{
}
/* drawing functions */
void clearScreen(int color)
{
   // default_gc->gops->fillRect(color,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,default_gc->buffer);
}

void drawPixel(int color,int x, int y)
{
    tstXY(x,y);
    //default_gc->gops->drawPixel(color, x, y,default_gc->buffer);
}

int readPixel(int x, int y)
{
    tstXY(x,y);
    //return default_gc->gops->readPixel(x,y,default_gc->buffer);
}

void drawRect(int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    //default_gc->gops->drawRect(color,x,y,width,height,default_gc->buffer);
}

void fillRect(int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    //default_gc->gops->fillRect(color,x,y,width,height,default_gc->buffer);
}

void drawLine(int color, int x1, int y1, int x2, int y2)
{
   /* int numpixels;
    int i;
    int deltax, deltay;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    tstXY(x1,y1);
    tstXY(x2,y2);

    deltax = abs(x2 - x1);
    deltay = abs(y2 - y1);

    if(deltax >= deltay)
    {
        numpixels = deltax;
        d = 2 * deltay - deltax;
        dinc1 = deltay * 2;
        dinc2 = (deltay - deltax) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    }
    else
    {
        numpixels = deltay;
        d = 2 * deltax - deltay;
        dinc1 = deltax * 2;
        dinc2 = (deltax - deltay) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }
    numpixels++;*/
     /* include endpoints */
/*
    if(x1 > x2)
    {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if(y1 > y2)
    {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for(i=0; i<numpixels; i++)
    {
        default_gc->gops->drawPixel(color, x, y,default_gc->buffer);

        if(d < 0)
        {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        }
        else
        {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }*/
}

void putS(int color, int bg_color, int x, int y, char *s)
{
    /*FONT_ID font=default_font;
    int len=strlen(s);
    char c;

    tstXY(x,y);

    if(font->width*len>SCREEN_WIDTH)
    {
        s[SCREEN_WIDTH/font->width]=0;
        c = s[SCREEN_WIDTH/font->width];
    }

    default_gc->gops->drawString(font,color,bg_color,x,y,s,default_gc->buffer);

    if(font->width*len>SCREEN_WIDTH)
    {
        s[SCREEN_WIDTH/font->width]=c;
    }*/
}

int getStringS(const unsigned char *str, int *w, int *h)
{
    /*FONT_ID font=default_font;

    return default_gc->gops->getStringSize(font,str,w,h);*/
}

void putC(int color, int bg_color, int x, int y, char s)
{
   /* FONT_ID font=default_font;

    tstXY(x,y);

    default_gc->gops->drawChar(font,color,bg_color,x,y,s,default_gc->buffer);*/
}
