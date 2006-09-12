/* BTN test but oxy77 */
/* Sept 2006          */

#include <api.h>
#include <evt.h>

#include <sys_def/colordef.h>
#include <kernel/io.h>
#include <kernel/osd.h>

int main_x,main_y,bmap_x,bmap_y;

void redraw(void)
{
    char txt[100];
    gfx_fillRect(COLOR_WHITE,1,30,350,70);
    sprintf(txt, "main: (%x,%x)",main_x,main_y);
    gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,30, txt);
    sprintf(txt, "bmap: (%x,%x)",bmap_x,bmap_y);
    gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,45, txt);
    outw(main_x,OSD_BITMAP0_SHIFT_HORIZ);
    outw(main_y,OSD_BITMAP0_SHIFT_VERT);
    outw(bmap_x,OSD_BITMAP0_X);
    outw(bmap_y,OSD_BITMAP0_Y);
}


void app_main(int argc,char * * argv)
{
    int evt;
    int evt_handler=evt_getHandler(BTN_CLASS);
    int stop=0;


    int mode=0;

    main_x=main_y=bmap_x=bmap_y=0;


    gfx_clearScreen(COLOR_WHITE);
    gfx_drawRect(COLOR_BLUE,0,0,352,240);

    gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,5, "Press OFF to exit");
    redraw();
    while(!stop)
    {
        evt=evt_getStatus(evt_handler);
        if(evt==NO_EVENT)
            continue;

        switch(evt)
        {
            case BTN_OFF:
                stop=1;
                break;
            case BTN_F1:
                mode=((mode==1)?0:1);
                if(mode)
                {
                    gfx_fillRect(COLOR_WHITE,1,15,350,15);
                    gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,15, "Mod Bmap position");
                }
                else
                {
                    gfx_fillRect(COLOR_WHITE,1,15,350,15);
                    gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,15, "Mod Main config");
                }
                break;
            case BTN_UP:
                if(mode)
                {
                    if(bmap_y) bmap_y--;
                }
                else
                {
                    if(main_y) main_y--;
                }
                redraw();
                break;
            case BTN_DOWN:
                if(mode)
                {
                    if(bmap_y<240) bmap_y++;
                }
                else
                {
                    if(main_y<240) main_y++;
                }
                redraw();
                break;
            case BTN_LEFT:
                if(mode)
                {
                    if(bmap_x) bmap_x--;
                }
                else
                {
                    if(main_x) main_x--;
                }
                redraw();
                break;
            case BTN_RIGHT:
                if(mode)
                {
                    if(bmap_x<352) bmap_x++;
                }
                else
                {
                    if(main_y<352) main_x++;
                }
                redraw();
                break;
        }
    }

}
