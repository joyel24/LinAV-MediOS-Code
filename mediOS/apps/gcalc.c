// gCalc v0.1
//by squidward

#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <api.h>
#include <evt.h>

#define LCD_WIDTH		220
#define LCD_HEIGTH		176
#define BAR_X			10
#define BAR_Y			40
#define BAR_WIDTH		200
#define BAR_HEIGTH		40
#define KEYPAD_X		60
#define KEYPAD_Y		90
#define KEYPAD_WIDTH		100
#define KEYPAD_HEIGTH		80
int w,h;
int sel = 0;
int weigth=1;
int i = 0;
int j = 0;
int num = 0;
int eqPressed = 0;
float inputnum = 0;
int seltonum[16] = {7,8,9,10,4,5,6,11,1,2,3,12,0,13,14,15};
int oper = 0;
float left = 0;
float outputnum = 0;
char ingfx_putStr[30];
char outgfx_putStr[30];

void initbackground(){
	gfx_clearScreen(COLOR_BLUE);
	
	i=0;
	j=0;
	int tmp;
	long quot;
	quot = LCD_WIDTH/LCD_HEIGTH;
	while(i<176){
		tmp = quot*i;
		j=0;
		while(j<tmp){
			int tmp2;
			tmp2 = rand()%220;
			gfx_drawPixel(COLOR_BLUE2,tmp2,i);
			j++;
		}
		i++;
	}

	gfx_fontSet(STD8X13);

	gfx_getStringSize("gCalc v0.1", &w, &h);
	gfx_putS(COLOR_WHITE,COLOR_BLUE,(LCD_WIDTH-w)/2,0,"gCalc v0.1");
	int h2=0;
	gfx_getStringSize("by SquidWard",&w,&h2);
	gfx_putS(COLOR_WHITE,COLOR_BLUE,(LCD_WIDTH-w)/2,h,"by SquidWard");
}
void initbar(){
	gfx_fillRect(COLOR_WHITE,BAR_X,BAR_Y,BAR_WIDTH,BAR_HEIGTH);
	gfx_drawLine(COLOR_LIGHT_GRAY,BAR_X,BAR_Y+20,BAR_X+BAR_WIDTH,BAR_Y+20);
	gfx_drawRect(COLOR_BLACK,BAR_X,BAR_Y,BAR_WIDTH,BAR_HEIGTH);
	gfx_drawRect(COLOR_BLACK,BAR_X-1,BAR_Y-1,BAR_WIDTH+2,BAR_HEIGTH+2);
	gfx_getStringSize("= ",&w,&h);
	gfx_putS(COLOR_BLACK,COLOR_WHITE,BAR_X+5,BAR_Y+20+(20-h)/2,"= ");
	}
void initkeypad(){

	//gfx_fillRect         (unsigned int color, int x, int y, int width, int height);
	//Clearing field...
	gfx_fillRect(COLOR_WHITE,KEYPAD_X,KEYPAD_Y,KEYPAD_WIDTH,KEYPAD_HEIGTH);
	gfx_drawRect(COLOR_BLACK,KEYPAD_X,KEYPAD_Y,KEYPAD_WIDTH,KEYPAD_HEIGTH);
	gfx_drawRect(COLOR_BLACK,KEYPAD_X-1,KEYPAD_Y-1,KEYPAD_WIDTH+2,KEYPAD_HEIGTH+2);

	i=0;
	while(i<6){
		gfx_drawLine(COLOR_BLACK,KEYPAD_X+20*i,KEYPAD_Y,KEYPAD_X+20*i,KEYPAD_Y+KEYPAD_HEIGTH);
		i++;
	}
	i=0;
	while(i<5){
		gfx_drawLine(COLOR_BLACK,KEYPAD_X,KEYPAD_Y+20*i,KEYPAD_X+KEYPAD_WIDTH-20,KEYPAD_Y+20*i);
		i++;
	}
	int x=0;
	int y=0;
	/* DOESNT WORK -> DONT KNOW WHY
	while(x<4){
		while(y<4){
			gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
			gfx_drawRect(COLOR_GREY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);
			gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+3,KEYPAD_Y+20*y+3,20-6,20-6);
			y++;
		}
		x++;
	}*/
	x=0;
	y=0;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=1;
	y=0;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=2;
	y=0;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=3;
	y=0;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=0;
	y=1;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=1;
	y=1;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=2;
	y=1;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=3;
	y=1;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=0;
	y=2;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=1;
	y=2;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=2;
	y=2;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=3;
	y=2;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=0;
	y=3;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=1;
	y=3;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=2;
	y=3;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);

	x=3;
	y=3;
	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+20*x+1,KEYPAD_Y+20*y+1,20-2,20-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+20*x+2,KEYPAD_Y+20*y+2,20-4,20-4);



	gfx_drawRect(COLOR_DARK_GREY,KEYPAD_X+80+1,KEYPAD_Y+1,20-2,80-2);
	gfx_drawRect(COLOR_LIGHT_GRAY,KEYPAD_X+80+2,KEYPAD_Y+2,20-4,80-4);

	w=5;
	h=12;
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+(20-w)/2,KEYPAD_Y+(20-h)/2,55);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+20+(20-w)/2,KEYPAD_Y+(20-h)/2,56);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+40+(20-w)/2,KEYPAD_Y+(20-h)/2,57);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+(20-w)/2,KEYPAD_Y+20+(20-h)/2,52);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+20+(20-w)/2,KEYPAD_Y+20+(20-h)/2,53);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+40+(20-w)/2,KEYPAD_Y+20+(20-h)/2,54);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+(20-w)/2,KEYPAD_Y+40+(20-h)/2,49);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+20+(20-w)/2,KEYPAD_Y+40+(20-h)/2,50);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+40+(20-w)/2,KEYPAD_Y+40+(20-h)/2,51);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+(20-w)/2,KEYPAD_Y+60+(20-h)/2,48);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+40+(20-w)/2,KEYPAD_Y+60+(20-h)/2,44);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+20+(20-w)/2,KEYPAD_Y+60+(20-h)/2,67);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+60+(20-w)/2,KEYPAD_Y+(20-h)/2,47);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+60+(20-w)/2,KEYPAD_Y+20+(20-h)/2,42);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+60+(20-w)/2,KEYPAD_Y+40+(20-h)/2,45);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+60+(20-w)/2,KEYPAD_Y+60+(20-h)/2,43);
	gfx_putC(COLOR_BLACK,COLOR_WHITE,KEYPAD_X+80+(20-w)/2,KEYPAD_Y+(KEYPAD_HEIGTH-h)/2,61);
}
void drawselected(int selected){
	initkeypad();
	switch(selected){
		/*Keypad:
		00|01|02|03|  |
		04|05|06|07|16|
		08|09|10|11|  |
		12|13|14|15|  |
		*/

		case 0:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X,KEYPAD_Y,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X-1,KEYPAD_Y-1,20+2,20+2);
			break;
		case 1:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20,KEYPAD_Y,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20-1,KEYPAD_Y-1,20+2,20+2);
			break;
		case 2:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40,KEYPAD_Y,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40-1,KEYPAD_Y-1,20+2,20+2);
			break;
		case 3:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60,KEYPAD_Y,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60-1,KEYPAD_Y-1,20+2,20+2);
			break;

		case 4:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X,KEYPAD_Y+20,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X-1,KEYPAD_Y+20-1,20+2,20+2);
			break;
		case 5:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20,KEYPAD_Y+20,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20-1,KEYPAD_Y+20-1,20+2,20+2);
			break;
		case 6:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40,KEYPAD_Y+20,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40-1,KEYPAD_Y+20-1,20+2,20+2);
			break;
		case 7:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60,KEYPAD_Y+20,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60-1,KEYPAD_Y+20-1,20+2,20+2);
			break;

		case 8:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X,KEYPAD_Y+40,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X-1,KEYPAD_Y+40-1,20+2,20+2);
			break;
		case 9:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20,KEYPAD_Y+40,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20-1,KEYPAD_Y+40-1,20+2,20+2);
			break;
		case 10:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40,KEYPAD_Y+40,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40-1,KEYPAD_Y+40-1,20+2,20+2);
			break;
		case 11:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60,KEYPAD_Y+40,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60-1,KEYPAD_Y+40-1,20+2,20+2);
			break;

		case 12:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X,KEYPAD_Y+60,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X-1,KEYPAD_Y+60-1,20+2,20+2);
			break;
		case 13:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20,KEYPAD_Y+60,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+20-1,KEYPAD_Y+60-1,20+2,20+2);
			break;
		case 14:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40,KEYPAD_Y+60,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+40-1,KEYPAD_Y+60-1,20+2,20+2);
			break;
		case 15:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60,KEYPAD_Y+60,20,20);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+60-1,KEYPAD_Y+60-1,20+2,20+2);
			break;
		case 16:
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+80,KEYPAD_Y,20,KEYPAD_HEIGTH);
			gfx_drawRect(COLOR_YELLOW,KEYPAD_X+80-1,KEYPAD_Y-1,20+2,KEYPAD_HEIGTH+2);
			break;

	}
}
void reactoninput(int sel){
	float tmpfloat;
	char tmpstr[30];
	switch(sel){
		case 0://Digit Pressed
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
		case 8:
		case 9:
		case 10:
		case 12:
		if(eqPressed==0){
			num = seltonum[sel];
			if(weigth==1){
				inputnum = 10*inputnum+num;
			}else{
				if(num!=0){
				tmpfloat=(float)num;
				tmpfloat=tmpfloat/weigth;
				inputnum = inputnum+tmpfloat;
				}
			weigth=10*weigth;
			}
			if((inputnum!=0)||(weigth!=1)){
				sprintf(tmpstr,"%1d",num);
				strcat(ingfx_putStr,tmpstr);
			}
		}
		break;
		case 14:// ',' Pressed
		if(eqPressed==0){
			if(weigth==1){
				strcat(ingfx_putStr,".\0");
				weigth=10*weigth;
			}
		}
		
		break;
		case 3://Operator Pressed
		case 7:
		case 11:
		case 15:
		if((oper==0)&&(inputnum!=0)){
			if(eqPressed==0){
			left = inputnum;
			}
			weigth = 1;
			inputnum = 0;
			if(sel==3){
				oper=1;
				strcat(ingfx_putStr,"/\0");
			}else if(sel==7){
				oper=2;
				strcat(ingfx_putStr,"*\0");
			}else if(sel==11){
				oper=3;
				strcat(ingfx_putStr,"-\0");
			}else if(sel==15){
				oper=4;
				strcat(ingfx_putStr,"+\0");
			}
			eqPressed=0;
		}
		
		break;
		case 16://'=' Pressed
		if((oper!=0)&&(inputnum!=0)&&(eqPressed==0)){
			initbar();
			strcpy(tmpstr,ingfx_putStr);
			strcpy(ingfx_putStr,"(\0");
			strcat(ingfx_putStr,tmpstr);
			strcat(ingfx_putStr,")\0");
			switch(oper){
				case 1:
				outputnum=left/inputnum;
				break;
				case 2:
				outputnum=left*inputnum;
				break;
				case 3:
				outputnum=left-inputnum;
				break;
				case 4:
				outputnum=left+inputnum;
				break;
			}
			eqPressed=1;
			left = outputnum;
			long int leftpart,fracpart;
			leftpart = (int)outputnum;
			tmpfloat = outputnum-leftpart;
			tmpfloat = 100000*tmpfloat+100000;
			fracpart = (int)tmpfloat;
			i=1;
			while(i<6){
				if(fracpart==(10*(fracpart/10))){
					fracpart = fracpart/10;
				}
				i++;
			}
			oper = 0;
			weigth=1;
			sprintf(outgfx_putStr,"%d",leftpart);
			if(fracpart!=1){
			strcat(outgfx_putStr,".");
			}
			sprintf(tmpstr,"%d",fracpart);
			i=0;
			unsigned int stringlen = strlen(tmpstr);
			while(i<stringlen){
				tmpstr[i]=tmpstr[i+1];
				i++;
			}
			strcat(outgfx_putStr,tmpstr);
			gfx_getStringSize(outgfx_putStr,&w,&h);
			gfx_putS(COLOR_BLACK,COLOR_WHITE,BAR_X+BAR_WIDTH-w-5,BAR_Y+20+(20-h)/2,outgfx_putStr);
		}
		
		break;
		case 13://'C' Pressed
		i = 0;
		j = 0;
		inputnum = 0;
		oper = 0;
		left = 0;
		eqPressed=0;
		weigth = 1;
		outputnum = 0;
		initbackground();

		initbar();
	
		strcpy(ingfx_putStr,"\0");
		strcpy(outgfx_putStr,"\0");
		gfx_getStringSize(ingfx_putStr,&w,&h);
		gfx_putS(COLOR_BLACK,COLOR_WHITE,BAR_X+BAR_WIDTH-w-5,BAR_Y+(20-h)/2,ingfx_putStr);
		drawselected(sel);
		break;
	}
	
	
	gfx_getStringSize(ingfx_putStr,&w,&h);
	gfx_putS(COLOR_BLACK,COLOR_WHITE,BAR_X+BAR_WIDTH-w-5,BAR_Y+(20-h)/2,ingfx_putStr);
	
}
void react(int but){
	switch(but)
					{
		/*Keypad:
		00|01|02|03|  |
		04|05|06|07|16|
		08|09|10|11|  |
		12|13|14|15|  |
		*/
						case 1: //up
							if((sel<16)&&(sel>3)){
								sel=sel-4;
							}
							drawselected(sel);
							break;
						case 2: //left
							if((sel==1)||(sel==2)||(sel==3)||(sel==5)||(sel==6)||(sel==7)||(sel==9)||(sel==10)||(sel==11)||(sel==13)||(sel==14)||(sel==15)){
								sel--;
							}else if(sel==16){
								sel=7;
							}
							drawselected(sel);
							break;
						case 3: //down
							if((sel<12)&&(sel>=0)){
								sel=sel+4;
							}
							drawselected(sel);
							break;
						case 4: //right
							if((sel==1)||(sel==2)||(sel==0)||(sel==5)||(sel==6)||(sel==4)||(sel==9)||(sel==10)||(sel==8)||(sel==13)||(sel==14)||(sel==12)){
								sel++;
							}else if((sel==3)||(sel==7)||(sel==11)||(sel==15)){
								sel=16;
							}
							drawselected(sel);
							break;
						case 5: //square
							reactoninput(sel);
							break;
						case 6: //cross

							break;
						case 7: //on

							break;
					}
	i=0;
	while(i<1000000){
		i++;
	}
	i=0;
}
void app_main(int argc,char ** argv)
{
    
    printf("\nIn othello\n");
    
    gfx_openGraphics();
	
	initbackground();

	initbar();
	
	strcpy(ingfx_putStr,"\0");
	strcpy(outgfx_putStr,"\0");
	gfx_getStringSize(ingfx_putStr,&w,&h);
	gfx_putS(COLOR_BLACK,COLOR_WHITE,BAR_X+BAR_WIDTH-w-5,BAR_Y+(20-h)/2,ingfx_putStr);
	gfx_getStringSize("= ",&w,&h);
	gfx_putS(COLOR_BLACK,COLOR_WHITE,BAR_X+5,BAR_Y+20+(20-h)/2,"= ");
	i = 0;
	j = 0;
	inputnum = 0;
	oper = 0;
	left = 0;
	eqPressed=0;
	weigth = 1;
	outputnum = 0;
	drawselected(sel);

    int stop_othello=0;
    printf("\nbefore loop\n");
    
//FIXME: variables related to the workaround for the broken get_evt() on the gmini
    int oldbutton;
    int newbutton;
    oldbutton = 0;
	//char debugtxt[10];
    while(!stop_othello)
    {
//FIXME: get_evt() never returns on the gmini so until it is fixed this work around is needed.
        newbutton = btn_readState();
				if(newbutton != oldbutton)
				{
					switch(newbutton)
					{
						case 0x0001: //up
							react(1);
							break;
						case 0x0004: //left
							react(2);
							break;
						case 0x0002: //down
							react(3);
							break;
						case 0x0008: //right
							react(4);
							break;
						case 0x0080: //square
							react(5);
							i=0;
							while(i<1000000){
							i++;
							}
							i=0;
							break;
						case 0x0100: //cross
							react(6);
							break;
						case 0x0200: //on
							react(7);
							break;
						case 0x0400: //off
							stop_othello=1;
							break;
					}
				}
				oldbutton = newbutton;
    }
    printf("\nafter loop\n");
    printf("\nout othello\n");
    
}
