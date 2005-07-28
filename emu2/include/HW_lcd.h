/* 
*   HW_lcd.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_LCD_H
#define __HW_LCD_H

#include <HW_mem.h>
//#include <mem_space.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

class mem_space;

class HW_lcd {
    public:
        HW_lcd(HW_mem * mem2);
        void updte_lcd(uint32_t base_addr);
        void drawPix(uint32_t addr,uint32_t val);     
        
        int nxtEvent(uint32_t addr);
          
    private:
        HW_mem * mem2;
        
        Display* display;
        Window window;
        GC gc;
        Colormap pal;
        int screen;
        XEvent event;
        
        int colorTab[256];
        
        void setPalette(int palette[256][3],int size);

};

#endif // __HW_LCD_H
