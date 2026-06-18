/*
* sudoku.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* uses grid generator from gSudoku by PdX
*/

#include <api.h>

#define SAVE_FILE "/medios/sudoku.dat"

int screen_width;
int screen_height;
int arch;

int plane_x;
int plane_y;

int evt_handler;

int difficulty=25;

static int helpers[81][10];

WIDGETMENU menu;
int menu_chosen;

////// GENERATE STUFF //////
bool haspair(int row[]);
bool hastriple(int row[]);
void rowpair(int row);
void colpair(int row);
void rowtriple(int row);
void elim();
void coltriple(int row);
int counts();
int oneleft(int cell);
void calcposs(int cell);
void solve();
bool cansolve();
void switchrow(int x, int y);
void switchcol(int x, int y);
void swap(int x, int y);
bool generate();
void printmenu(char *one, char *two, char *three);

__IRAM_DATA static int solvedtemplate[81] = {4, 5, 7, 3, 6, 1, 9, 2, 8,
						 2, 8, 3, 4, 9, 7, 6, 5, 1,
						 1, 6, 9, 8, 5, 2, 3, 7, 4,
						 7, 3, 8, 6, 4, 9, 2, 1, 5,
						 9, 4, 5, 1, 2, 3, 7, 8, 6,
						 6, 2, 1, 5, 7, 8, 4, 9, 3,
						 3, 1, 2, 7, 8, 4, 5, 6, 9,
						 5, 9, 4, 2, 1, 6, 8, 3, 7,
						 8, 7, 6, 9, 3, 5, 1, 4, 2 };

__IRAM_DATA static int grid[81];
__IRAM_DATA static int solved[81];
__IRAM_DATA static int solution[81];

__IRAM_DATA static int solgrid[81][10];
__IRAM_DATA static int triplea;
__IRAM_DATA static int tripleb;
__IRAM_DATA static int triplec;
__IRAM_DATA static int paira;
__IRAM_DATA static int pairb;

void my_putC(int color,int x,int y,unsigned char c){
    int bg;

    bg=gfx_readPixel(x+1,y+1);

    gfx_putC(color,bg,x,y,c);
};

void draw_grid(){
    int i,j,pos;
    int color;

    //grid
    for(i=2;i<=173;i+=19){
        gfx_drawLine(COLOR_GRAY,i,2,i,9*19+2);
        gfx_drawLine(COLOR_GRAY,2,i,9*19+2,i);
    }

    for(i=2;i<=173;i+=19*3){
        gfx_drawLine(COLOR_BLACK,i,2,i,9*19+2);
        gfx_drawLine(COLOR_BLACK,2,i,9*19+2,i);
    }

    //numbers
    for(j=0;j<9;j++){
        for(i=0;i<9;i++){
            pos=9*j+i;

            if(solved[pos]){

                //draw num

                gfx_fontSet(STD8X13);

                color=COLOR_BLACK;
                if(grid[pos]==0) color=COLOR_BLUE;

                my_putC(color,19*i+8,19*j+6,solved[pos]+'0');

            }else{

                //draw helpers

                int hnum=helpers[pos][0];
                int k;
                int hx,hy;

                if(hnum>0){

                    gfx_fontSet(STD4X6);

                    hx=0;
                    hy=0;

                    for(k=1;k<=hnum;++k){
                        my_putC(COLOR_GREEN,19*i+4+hx*6,19*j+4+hy*6,helpers[pos][k]+'0');

                        hx++;
                        if(hx>2){
                            hx=0;
                            hy++;
                        }
                    }
                }
            }
        }
    }
}


void draw_numbers(){

    gfx_fontSet(STD8X13);

    //grid
    gfx_drawRect(COLOR_LIGHT_GRAY,0,0,64,64);
    gfx_drawRect(COLOR_DARK_GRAY,1,1,62,62);
    gfx_drawRect(COLOR_BLACK,2,2,60,60);
    gfx_drawRect(COLOR_BLACK,22,2,20,60);
    gfx_drawRect(COLOR_BLACK,2,22,60,20);

    //numbers
    my_putC(COLOR_BLACK,2+7,2+4,'1');
    my_putC(COLOR_BLACK,2+19+7,2+4,'2');
    my_putC(COLOR_BLACK,2+38+7,2+4,'3');
    my_putC(COLOR_BLACK,2+7,2+19+4,'4');
    my_putC(COLOR_BLACK,2+19+7,2+19+4,'5');
    my_putC(COLOR_BLACK,2+38+7,2+19+4,'6');
    my_putC(COLOR_BLACK,2+7,2+38+4,'7');
    my_putC(COLOR_BLACK,2+19+7,2+38+4,'8');
    my_putC(COLOR_BLACK,2+38+7,2+38+4,'9');
}

int x1=1,x2=1,y1=1,y2=1;

int navigate_grid(bool *helper,bool *clear){
    bool sel2;
    int res;
    int evt;
    int pos;
    bool first;

    gfx_setPlane(BMAP1);

    *helper=false;
    *clear=false;

    sel2=false;
    first=true;
    res=-1;

    do{
        evt=evt_getStatus(evt_handler);

        if(!first){
            if(!evt) continue;

            pos=(y1*3+y2)*9+x1*3+x2;

            switch(evt){
                case BTN_UP:
                    if(sel2)
                        --y2;
                    else{
                        --y1;
                        x2=y2=1;
                    }
                    break;
                case BTN_DOWN:
                    if(sel2)
                        ++y2;
                    else{
                        ++y1;
                        x2=y2=1;
                    }
                    break;
                case BTN_LEFT:
                    if(sel2)
                        --x2;
                    else{
                        --x1;
                        x2=y2=1;
                    }
                    break;
                case BTN_RIGHT:
                    if(sel2)
                        ++x2;
                    else{
                        ++x1;
                        x2=y2=1;
                    }
                    break;
                case BTN_1:
                case BTN_ON:
                case BTN_F3:
                    if(!sel2){
                        sel2=true;
                    }else{
                        //ok only if not a base grid number
                        if(!grid[pos]){
                            *helper=(evt==BTN_F3);
                            res=pos;
                        }
                    }
                    break;
                case BTN_2:
                case BTN_OFF:
                    if(sel2){
                        sel2=false;
                    }else{
                        res=0;
                    }
                    break;
                case BTN_F2:
                    if(sel2){
                        //ok only if not a base grid number
                        if(!grid[pos]){
                            *clear=true;
                            res=pos;
                        }
                    }
                default:
                    continue;
            }
        }

        first=false;

        x1=(x1+3)%3;
        y1=(y1+3)%3;
        x2=(x2+3)%3;
        y2=(y2+3)%3;

        //clear
        gfx_clearScreen(COLOR_WHITE);

        //selection
        if(sel2){
            gfx_fillRect(COLOR_LIGHT_BLUE,2+(x1*3+x2)*19,2+(y1*3+y2)*19,19,19);
        }else{
            gfx_fillRect(COLOR_LIGHT_BLUE,2+x1*57,2+y1*57,57,57);
        }

        draw_grid();

    }while(res<0);

    return res;
}



int choose_number(){
    int x,y;
    int res;
    int evt;
    bool first;

    gfx_planeSetSize(BMAP2,64,64,8);
    gfx_planeSetPos(BMAP2,plane_x+screen_width-64,plane_y+(screen_height-64)/2);
    gfx_planeShow(BMAP2);

    gfx_setPlane(BMAP2);

    x=1;
    y=1;
    res=-1;
    first=true;

    do{
        evt=evt_getStatus(evt_handler);

        if(!first){
            if(!evt) continue;

            switch(evt){
                case BTN_UP:
                    --y;
                    break;
                case BTN_DOWN:
                    ++y;
                    break;
                case BTN_LEFT:
                    --x;
                    break;
                case BTN_RIGHT:
                    ++x;
                    break;
                case BTN_1:
                case BTN_ON:
                case BTN_F3:
                    res=y*3+x+1;
                    break;
                case BTN_2:
                case BTN_OFF:
                    res=0;
                    break;
                default:
                    continue;
            }
        }

        first=false;

        x=(x+3)%3;
        y=(y+3)%3;

        //clear
        gfx_clearScreen(COLOR_WHITE);

        //selection
        gfx_fillRect(COLOR_LIGHT_BLUE,2+x*20,2+y*20,20,20);
        
        draw_numbers();

    }while(res<0);

    gfx_planeHide(BMAP2);

    return res;
}


void grid_init(){
    int j;

    for(j=0;j<81;j++){
        grid[j]=0;
        solved[j]=0;
        solution[j]=0;
        helpers[j][0]=0;
    }
}

void game_loop(){
    bool helper;
    bool clear;
    int pos;
    int num;

    do{
        pos=navigate_grid(&helper,&clear);

        if(pos>0){

            if(clear){

                if(solved[pos]){ // number?
                    // remove it
                    solved[pos]=0;
                }else{
                    // remove last helper
                    if(helpers[pos][0]){
                        --helpers[pos][0];
                    }
                }

            }else{

                num=choose_number();

                if(num>0){
                    if(helper){
                        int i;
                        bool ok;

                        // helper already added?
                        ok=true;
                        for(i=1;i<=helpers[pos][0];++i){
                            if (helpers[pos][i]==num){
                                ok=false;
                                break;
                            }
                        }

                        if(ok){
                            // add new helper
                            int hpos;
                            hpos=++helpers[pos][0];
                            helpers[pos][hpos]=num;
                        }

                    }else{

                        // add the number
                        solved[pos]=num;

                    }
                }
            }
        }
    }while(pos>0);
}

void menu_onClick(WIDGETMENU m, WIDGETMENU_ITEM mi){
    if(!strcmp(mi->caption,"Generate grid & play")){
        menu_chosen=1;
    }else if(!strcmp(mi->caption,"Load grid")){
        menu_chosen=2;
    }else if(!strcmp(mi->caption,"Save grid")){
        menu_chosen=3;
    }else if(!strcmp(mi->caption,"Show solution")){
        menu_chosen=4;
    }else if(!strcmp(mi->caption,"Return to game")){
        menu_chosen=5;
    }
}

void menu_init(){
    WIDGETMENU_TRACKBAR mt;
    WIDGETMENU_ITEM mi;

    menu=widgetMenu_create();
    menu->setRect(menu,0,0,screen_width,screen_height);
    menu->ownItems=true;
    menu->onClick=(MENU_CLICKEVENT)menu_onClick;

    mt=widgetMenuTrackbar_create();
    mt->caption="Difficulty";
    mt->trackbar->minimum=1;
    mt->trackbar->maximum=60;
    mt->trackbar->value=30;
    menu->addItem(menu,mt);

    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Generate grid & play";
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Load grid";
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Save grid";
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Show solution";
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Return to game";
    menu->addItem(menu,mi);
}

bool menu_execute(){
    int event;
    int f;

    menu_chosen=0;

    menu->handleEvent(menu,EVT_REDRAW);

    do
    {
        event=evt_getStatus(evt_handler);
        if (!event) continue; // no new events

        menu->handleEvent(menu,event);
    }
    while(event!=BTN_OFF && !menu_chosen);

    difficulty=81-menu->getTrackbar(menu,menu->indexFromCaption(menu,"Difficulty"))->value;

    //generate
    if(menu_chosen==1){
        grid_init();
        generate();
    }

    //save
    if(menu_chosen==2){
        gfx_clearScreen(COLOR_WHITE);
        gfx_putS(COLOR_BLACK,COLOR_WHITE,0,0,"Loading...");

        f=open(SAVE_FILE,O_RDONLY);
        if(f>=0){
            read(f,grid,sizeof(grid));
            read(f,solved,sizeof(solved));
            read(f,solution,sizeof(solution));
            read(f,helpers,sizeof(helpers));
            close(f);
        }
    }

    //save
    if(menu_chosen==3){
        gfx_clearScreen(COLOR_WHITE);
        gfx_putS(COLOR_BLACK,COLOR_WHITE,0,0,"Saving...");

        f=open(SAVE_FILE,O_RDWR|O_CREAT);
        if(f>=0){
            write(f,grid,sizeof(grid));
            write(f,solved,sizeof(solved));
            write(f,solution,sizeof(solution));
            write(f,helpers,sizeof(helpers));
            close(f);
        }
    }

    //show solution
    if(menu_chosen==4){
        memcpy(solved,solution,sizeof(solved));
    }

    return menu_chosen!=0;
}

void welcome_screen(){
    int y=0;

    gfx_setPlane(BMAP1);
    gfx_clearScreen(COLOR_WHITE);
	gfx_fontSet(STD6X9);

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y,       "Sudoku");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "======");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "by GliGli, grid generator by PdX");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Ingame keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:     Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On/Btn1:   Select/write number");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off/Btn2:  Deselect/menu");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:        Write helper");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:        Clear number/helper");


    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,screen_height-10,     "Press a key to continue...");

    while(!evt_getStatus(evt_handler));
}

void app_main(int argc,char* argv)
{
    bool ok;

    arch=getArch();
    getResolution(&screen_width,&screen_height);

    evt_handler=evt_getHandler(BTN_CLASS);

    gfx_openGraphics();
    gfx_planeGetPos(BMAP2,&plane_x,&plane_y);
    gfx_clearScreen(COLOR_WHITE);

    grid_init();

    menu_init();

    welcome_screen();

    ok=true;
    for(;;){

        ok=menu_execute();

        if(!ok) break;

        game_loop();

    };

    evt_freeHandler(evt_handler);
}


//*****************************************************************************
//*****************************************************************************
//SOLVER FUNCTIONS
//*****************************************************************************
//*****************************************************************************

bool haspair(int row[])
{
	paira=0; pairb=0;
	int count=0, i;
	for(i=0;i<9;i++)
	{
		if(row[i]!=0)
		{
			count++;
			if(paira==0)
				paira=row[i];
			else
				pairb=row[i];
		}
	}
	if(count==2)
	{
		return true;
	}
	else
		return false;
}

bool hastriple(int row[])
{
	int count=0,i;
	triplea=0; tripleb=0; triplec=0;
	for(i=0;i<9;i++)
	{
		if(row[i]!=0)
		{
			count++;
			if(triplea==0)
				triplea=row[i];
			else if (tripleb==0)
				tripleb=row[i];
			else triplec=row[i];
		}
	}
	if(count==3)
	{
		return true;
	}
	else
		return false;
}


void rowpair(int row)
{
	int a=0, b=0, c=0, d=0, i, j, k;
	for(i=0;i<9;i++)
	{
		a=0; b=0; c=0; d=0;
		if(haspair(solgrid[row*9+i]))
		{
			c=paira; d=pairb;
			for(j=0;j<9;j++)
			{
				if(haspair(solgrid[row*9+j])&&i!=j)
				{
					if(paira==c&&pairb==d)
					{
						for(k=0;k<9;k++)
							if(k!=i&&k!=j)
							{
								solgrid[row*9+k][paira-1]=0;
								solgrid[row*9+k][pairb-1]=0;
							}
					}
				}
			}
		}
	}
}

void colpair(int row)
{
	int a=0, b=0, c=0, d=0, i, j, k;
	for(i=0;i<9;i++)
	{
		a=0; b=0; c=0; d=0;
		if(haspair(solgrid[i*9+row]))
		{
			c=paira; d=pairb;
			for(j=0;j<9;j++)
			{
				if(haspair(solgrid[j*9+row])&&i!=j)
				{
					if(paira==c&&pairb==d)
					{
						for(k=0;k<9;k++)
							if(k!=i&&k!=j)
							{
								solgrid[k*9+row][paira-1]=0;
								solgrid[k*9+row][pairb-1]=0;
							}
					}
				}
			}
		}
	}
}

void rowtriple(int row)
{

	int a=0, b=0, c=0, d=0, e=0, f=0, i, j, k, l;
	for(i=0;i<9;i++)
	{
		a=0; b=0; c=0;
		if(hastriple(solgrid[row*9+i]))
		{
			d=triplea; e=tripleb; f=triplec;
			for(j=0;j<9;j++)
			{
				if(hastriple(solgrid[row*9+j])&&i!=j)
				{
					if(triplea==d&&tripleb==e&&triplec==f)
					{
						for(k=0;k<9;k++)
						{
							if(hastriple(solgrid[row*9+k])&&i!=k&&k!=j)
							{
								if(triplea==d&&tripleb==e&&triplec==f)
									for(l=0;l<9;l++)
									 if(l!=i&&l!=j&&l!=k)
									 {
									solgrid[row*9+l][triplea-1]=0;
									solgrid[row*9+l][tripleb-1]=0;
									solgrid[row*9+l][triplec-1]=0;
									 }

							}
						}
					}
				}
			}
		}
	}
}

void elim()
{
	int num, x, y, i, j, b, count=0;
	
	
	for(num=1;num<10;num++)
	{
		
		for(x=0;x<3;x++) 
			for(y=0;y<3;y++)
			{
				count=0; b=0;
				for(j=0;j<9;j++) //eachcell in box
				{
					if(num==solgrid[ (y*3+j/3)*9 + (x*3+(j%3)) ][num-1])
					{
						count++;
						b=j;
					}
				}
				if(count==1)         //if singular is found
					for(j=0;j<9;j++)
						if(j!=num-1)
						{
							grid[ (y*3+b/3)*9 + (x*3+(b%3)) ] = num;
							for(i=0;i<0;i++)
								solgrid[ (y*3+b/3)*9 + (x*3+(b%3)) ][i] = 0;
						}
			}
	}
}

void coltriple(int row)
{

	int a=0, b=0, c=0, d=0, e=0, f=0, i, j, k, l;
	for(i=0;i<9;i++)
	{
		a=0; b=0; c=0;
		if(hastriple(solgrid[i*9+row]))
		{
			d=triplea; e=tripleb; f=triplec;
			for(j=0;j<9;j++)
			{
				if(hastriple(solgrid[j*9+row])&&i!=j)
				{
					if(triplea==d&&tripleb==e&&triplec==f)
					{
						for(k=0;k<9;k++)
						{
							if(hastriple(solgrid[k*9+row])&&i!=k&&k!=j)
							{
								if(triplea==d&&tripleb==e&&triplec==f)
									for(l=0;l<9;l++)
									 if(l!=i&&l!=j&&l!=k)
									 {
									solgrid[l*9+row][triplea-1]=0;
									solgrid[l*9+row][tripleb-1]=0;
									solgrid[l*9+row][triplec-1]=0;
									 }

							}
						}
					}
				}
			}
		}
	}
}


int counts()
{
	int mycount=0, i;
	for(i=0;i<81;i++)
		if (grid[i] != 0) mycount++;
	return mycount;
}




int oneleft(int cell)
{
	int count=0, num=0,i;

	for(i=0; i<9; i++)
	{
		if(solgrid[cell][i]!=0)
		{
			count++;
			num = solgrid[cell][i];
		}
	}

	if(count!=1)
		return 0;
	else
		return num;
}



void calcposs(int cell)
{
	bool rowtest=true;
	int row = cell/9;
	int col = cell%9;
	int boxx = (cell / 3)%3;
	int boxy = cell / 27;
	int i,j;
	for(i=0; i<9; i++)
	{
		rowtest = true;
		for(j=0;j<9;j++)
		{
			if( (grid[j*9+col]==i+1||grid[row*9+j]==i+1|| grid[ (boxy*3+j/3)*9 + (boxx*3+(j%3)) ]==i+1) )
				rowtest = false;
		}
		if(rowtest&&grid[cell]==0)
			solgrid[cell][i] = i+1;
		else
			solgrid[cell][i] = 0;
	}
}

void solve()
{
	int x=0 ,i, j, cell;

	for(i=0;i<81;i++)
		for(j=0;j<9;j++)
			solgrid[i][j]=0;

	for(cell=0; cell<81; cell++)
	{

		calcposs(cell);
		rowpair(cell%9);
		rowtriple(cell%9);
		colpair(cell/9);
		coltriple(cell/9);
		x = oneleft(cell);
		if(x!=0)
		{
			grid[cell]=x;

			cell=-1;
		}
	}
}

bool cansolve()
{
	int oldcount=0;
		while(counts()!=oldcount)
		{
		oldcount=counts();
		solve();
		elim();
		}	
		
		if (oldcount==81)
			return true;
		else
			return false;
}

//*****************************************************************************
//*****************************************************************************
//GENERATE FUNCTIONS
//*****************************************************************************
//*****************************************************************************

void switchrow(int x, int y)
{
	int temp, i;
	for(i=0;i<9;i++)
	{
		temp = solved[x*9+i];
		solved[x*9+i] = solved[y*9+i];
		solved[y*9+i] = temp;
	}
}

void switchcol(int x, int y)
{
	int temp,i;
	for(i=0;i<9;i++)
	{
		temp = solved[i*9+x];
		solved[i*9+x] = solved[i*9+y];
		solved[i*9+y] = temp;
	}
}

void swap(int x, int y)
{
	int i;
	for(i=0;i<81;i++)
	{
		if(solved[i]==x)
			solved[i]=y;
		else if (solved[i]==y)
			solved[i]=x;
	}
}


bool generate()
{
	int x, y, ran, i, j,count;

	printf("generate start\n");
	
	//drawing stuff
    gfx_setPlane(BMAP1);
    gfx_clearScreen(COLOR_WHITE);
	gfx_fontSet(STD8X13);
	gfx_putS(COLOR_BLACK,COLOR_WHITE,(screen_width-18*8)/2,screen_height/4-13/2,"Generating grid...");
    gfx_drawRect(COLOR_BLACK,0,(screen_height-30)/2,screen_width,30);

	for(i=0;i<81;i++)
	{
		solved[i]=solvedtemplate[i];
		grid[i]=0;
		for(j=0;j<9;j++)
			solgrid[i][j]=0;
	}
	srand((unsigned int) tmr_getTick());

	//generate solution
	for(i=0;i<50;i++)
	{
		x = rand() % 9;
		y = rand() % 3 + (x/3)*3;

		if(rand()%2)
			switchrow(x,y);
		else
			switchcol(x,y);
		swap(rand()%9+1, rand()%9+1);
	}

	int tries=0;

	for(i=0;i<81;i++)
	{
		grid[i]=solved[i];
		solution[i]=solved[i];
	}

    count=counts();

	while(count>difficulty)
	{
    	//drawing advance
        gfx_fillRect(COLOR_LIGHT_BLUE,1+screen_width*(81-count)/(81-difficulty),(screen_height-28)/2,screen_width/(81-difficulty)+1,28);

        //off pressing
        if(evt_getStatus(evt_handler)==BTN_OFF) return false;

		tries++;
		ran = rand()%81;
		if(grid[ran]!=0)
		{
		grid[ran] = 0;
		if(cansolve())
		{
			solved[ran]=0;
		}
		if(tries==300)
			return false;
		}
		for(j=0;j<81;j++)
			grid[j]=solved[j];

        count=counts();
	}

	printf("\ngenerate end\n");

	return true;
}

