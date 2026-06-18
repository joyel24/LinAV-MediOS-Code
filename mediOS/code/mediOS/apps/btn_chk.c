/* BTN test but oxy77 */
/* Sept 2006          */

#include <api.h>
#include <evt.h>

#include <sys_def/colordef.h>

int coord_x[16] = {25,25,15,40,5,20,35,50,5,20,35,50,60,60,60,60 };
int coord_y[16] = {20,40,30,30,50,50,50,50,5,5,5,5,15,25,35,45 };
char * name_str[16] = {"Up","Dwn", "Lft", "Rgt", "F1", "F2", "F3", "F4", "B1", "B2", "B3", "B4", "ON", "OFF", "Opt1", "Opt2" };

int cur_disp[16];
int new_disp[16];

void print_and_save()
{
	int i;
	for(i=0;i<16;i++)
	{
		if(cur_disp[i]!=new_disp[i])
		{
			if(new_disp[i])
			{
				gfx_putS(COLOR_BLACK, COLOR_WHITE, coord_x[i],coord_y[i], name_str[i]);
			}
			else
			{
				int w,h;
				gfx_getStringSize(name_str[i],&w,&h);
				gfx_fillRect(COLOR_WHITE,coord_x[i],coord_y[i],w,h);
			}
			cur_disp[i]=new_disp[i];
		}
	}
}

void app_main(int argc,char * * argv)
{
	int evt;
	int evt_handler=evt_getHandler(BTN_CLASS);
	int stop=0;
	int started=0;
	int i;
	int off_pressed=0;
	
	int cur_disp[16];
	
	for(i=0;i<16;i++) { cur_disp[i]=0; new_disp[i]=1;}
	
    gfx_clearScreen(COLOR_WHITE);

	print_and_save();
	gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,60, "Press a btn to start");

	while(!stop)
	{
		
		if(!started)
		{
			evt=evt_getStatus(evt_handler);
			if(evt!=NO_EVENT)
			{
				int w,h;
				started=1;
				for(i=0;i<16;i++) { new_disp[i]=0;}
				print_and_save();
				gfx_getStringSize("Press a btn to start",&w,&h);
				gfx_fillRect(COLOR_WHITE,5,60,w,h);
				gfx_putS(COLOR_BLACK, COLOR_WHITE, 5,60, "Started press 3 times OFF to exit");
			}
		}
		else
		{
			evt=evt_getStatus(evt_handler);		
			
			
			for(i=0;i<16;i++) { new_disp[i]=0;}
			if(evt!=NO_EVENT)
			{
				if(evt==BTN_OFF)
				{
					off_pressed++;
					if(off_pressed>=3)
						stop=1;
				}
				else
					off_pressed=0;
				new_disp[evt-1]=1;
				print_and_save();
			}
		}
	}
    
}
