#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"
#include "graphics.h"
#include "events.h"
#include "alias.h"

/***********************
 * Miscellaneous DEFINEs
 **********************/
#define MP3_BUFF_SIZE (1020*1000)
#define MWINCLUDECOLORS

#define COLOR_WHITE     16
#define COLOR_GRAY      20
#define COLOR_BLACK     1
#define COLOR_BLUE      86
#define COLOR_DARKBLUE  55
#define COLOR_RED       13
#define COLOR_GREEN     195
#define COLOR_DARKGREEN 159
#define COLOR_YELLOW    15
#define COLOR_ORANGE    42

/******************************
 * Color order for LCD printing
 *****************************/
const char *colortext[] = {
"White     ",
"Gray      ",
"Black     ",
"Blue      ",
"Dark Blue ",
"Red       ",
"Green     ",
"Dark Green",
"Yellow    ",
"Orange    "
};

/*******************************
 * And their corresponing colors
 ******************************/
const char colortable[] = {
COLOR_WHITE,
COLOR_GRAY,
COLOR_BLACK,
COLOR_BLUE,
COLOR_DARKBLUE,
COLOR_RED,
COLOR_GREEN,
COLOR_DARKGREEN,
COLOR_YELLOW,
COLOR_ORANGE
};

/************
 * Fonts used
 ***********/
needFont(std6x9);
needFont(std7x13);

/*************************
 * Ints for PLAYBACK/SOUND
 ************************/
/* sound settings */
int vol, bass = 50, treb = 50, bal = 50, loud = 0;
int oldvol;                  /* used for pause/resume */
int wait, end;               /* used for playback */
int fd_dsp, fd_file, fd_mix; /* used for playback */
int pause = 0;               /* 1 if paused */
int settings_applied = 0;    /* "1" if settings have been applied */
int evt;                     /* button reading */
int fade = 1;                /* fade on stop/pause */

/*******************
 * Ints for GRAPHICS
 ******************/
int peakmeters = 1;  /* "1" for peak meters, "0" for oscillograph */
int scroll_osci = 1; /* "1" if scrolling instead of looping */
int osci_x = 0;      /* used for looping mode */
int lpos, rpos;      /* used to smoothen peak meters */
int peak_decay = 3;  /* number of pixels to decrease for peak meter */

/*****************
 * Ints for COLORS
 ****************/
int peak_levelcolor = 3; /* color of peak meter bars 0-9 */
int peak_bgcolor = 4;    /* bg color of peak meters  0-9 */
int osci_levelcolor = 3; /* color of peaks for oscillograph 1-10 */
int osci_bgcolor = 4;    /* color of bg for oscillograph    1-10 */

/******************
 * Ints for WINDOWS
 *****************/
int window = 1;                   /* "1" = main, "2" = settings, "3" = sound */
int sound_cursor_position = 1;    /* cursor position at sound settings */
int settings_cursor_position = 1; /* cursor position at settings */
int main_drawn = 0;               /* "1" if has been drawn */
int settings_drawn = 0;           /* "1" if has been drawn */
int soundsettings_drawn = 0;      /* "1" if has been drawn */

/*********************
 * Miscellaneous CHARs
 ********************/
char defFilename[]="/mnt/file.mp3"; /* default file location */
char * filename;                    /* current file location */
char tmp[60];                       /* used for printing text */

/*******************
 * Other
 ******************/
struct mp3_play data; /* mp3 data */
struct av_peak av_p;  /* left/right levels */

/***************************************
 * Draws a progress bar on a 0-100 scale
 **************************************/
void draw_settings_progressbar(int x, int y, int value)
{
    int j;
    /*
    lcd_drawrect(COLOR_WHITE, x, y, 104, 9);
    lcd_fillrect(COLOR_BLACK, x+value+2, y+2, 99-value, 5);
    lcd_fillrect(COLOR_BLUE,  x+2, y+2, value, 5);*/

    for(j=y+5; j<y+7; j++)
    {
        lcd_drawline(COLOR_BLACK, x+value, j, x+value+(100-value), j);
        lcd_drawline(COLOR_BLUE, x, j, x+value, j);
    }

    lcd_drawrect(COLOR_WHITE, x, y+3, 101, 6);
}

/**************************
 * Draw sound settings text
 *************************/
void draw_soundsettings(void)
{
    sprintf(tmp,"Vol: %03d%%",vol);
    if(sound_cursor_position == 1)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 100, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 100, tmp);

    sprintf(tmp,"Bass: %03d%%",bass);
    if(sound_cursor_position == 2)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 115, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 115, tmp);

    sprintf(tmp,"Treble: %03d%%",treb);
    if(sound_cursor_position == 3)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 130, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 130, tmp);

    sprintf(tmp,"Balance: %03d%%",bal);
    if(sound_cursor_position == 4)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 145, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 145, tmp);

    sprintf(tmp,"Loudness: %03d%%",loud);
    if(sound_cursor_position == 5)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 160, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 160, tmp);

    /*
    draw_settings_progressbar(100, 99, vol);
    draw_settings_progressbar(100, 114, bass);
    draw_settings_progressbar(100, 129, treb);
    draw_settings_progressbar(100, 144, bal);
    draw_settings_progressbar(100, 159, loud);
    */
}

/********************
 * Draw settings text
 *******************/
void draw_settings(void)
{
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 40, "--- GENERAL OPTIONS ---");

    if(fade == 1)
        sprintf(tmp, "Fade On Stop/Pause: Yes");
    else
        sprintf(tmp, "Fade On Stop/Pause: No ");
    if(settings_cursor_position == 1)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 55, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 55, tmp);

    /*
     *VISUALIZATION CATEGORY
     */
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 85, "--- VISUALIZATION OPTIONS ---");

    if(peakmeters == 1)
        sprintf(tmp, "Visualization: Peak Meters ");
    else
        sprintf(tmp, "Visualization: Oscillograph");
    if(settings_cursor_position == 2)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 100, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 100, tmp);

    if(scroll_osci == 1)
        sprintf(tmp, "Scrolling Oscillograph: Yes");
    else
        sprintf(tmp, "Scrolling Oscillograph: No ");
    if(settings_cursor_position == 3)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 115, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 115, tmp);

    if(peak_decay > 0)
        sprintf(tmp, "Peak Release (Peak Meter): %02d        ", peak_decay);
    else
        sprintf(tmp, "Peak Release (Peak Meter): [No Decay]");
    if(settings_cursor_position == 4)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 130, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 130, tmp);

    /*
     * COLORS CATEGORY
     */
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 160, "--- COLOR OPTIONS ---");

    sprintf(tmp, "Peak Meter Color [Level]: %s", colortext[peak_levelcolor]);
    if(settings_cursor_position == 5)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 175, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 175, tmp);

    sprintf(tmp, "Peak Meter Color [Background]: %s", colortext[peak_bgcolor]);
    if(settings_cursor_position == 6)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 190, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 190, tmp);

    sprintf(tmp, "Oscillograph Color [Level]: %s", colortext[osci_levelcolor]);
    if(settings_cursor_position == 7)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 205, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 205, tmp);

    sprintf(tmp, "Oscillograph Color [Background]: %s", colortext[osci_bgcolor]);
    if(settings_cursor_position == 8)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 220, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 220, tmp);

}

/********************
 * Apply the settings
 *******************/
void apply_settings(void)
{
    ioctl(fd_mix,AV_SET_MIX_VOLUME,&vol);
    ioctl(fd_mix,AV_SET_MIX_BASS,&bass);
    ioctl(fd_mix,AV_SET_MIX_TREBLE,&treb);
    ioctl(fd_mix,AV_SET_MIX_BALANCE,&bal);
    ioctl(fd_mix,AV_SET_MIX_LOUDNESS,&loud);
}

/**************************
 * Draw text along the side
 *************************/
void draw_main_help_text(void)
{
    if(pause == 0)
    {
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 16, "Pause    ");
    }
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 16, "Resume");

    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275,  41, "Stop");

    sprintf(tmp,"Vol:%03d",vol);
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275,  96, tmp);

    /* lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 160, "Help"); */
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 181, "Options");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 211, "Sound");

    lcd_drawrect(COLOR_WHITE, 272,  14, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272,  39, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272,  94, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 149, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 179, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 209, (320-272), 13);
}

/**************************
 * Draw text along the side
 *************************/
void draw_settings_help_text(void)
{
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 0, 0,
               "UP/DOWN: Move cursor. LEFT/RIGHT: Change value.");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 0, 10, "OFF/F2: Exit");

    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275,  41, "Back");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 181, "Back");

    lcd_drawrect(COLOR_WHITE, 272,  14, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272,  39, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 149, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 179, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 209, (320-272), 13);
}

/**************************
 * Draw text along the side
 *************************/
void draw_soundsettings_help_text(void)
{
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 0, 0,
               "UP/DOWN: Move cursor. LEFT/RIGHT: Change value.");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 0, 10, "OFF/F3: Exit");

    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275,  41, "Back");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 211, "Back");

    lcd_drawrect(COLOR_WHITE, 272,  14, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272,  39, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 149, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 179, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 209, (320-272), 13);
}

/**********************
 * Pause or resume song
 *********************/
void pause_resume(void)
{
    if(pause) /* let's resume */
    {
        ioctl(fd_dsp,AV_DSP_START_MP3,NULL);
        pause=0;

        if(fade) /* fade in */
        {
            oldvol = vol;
            vol = 30;
            while(vol < oldvol)
            {
                vol++;
                apply_settings();
            }
        }
    }
    else /* let's pause */
    {
        if(fade) /* fade out */
        {
            oldvol = vol;
            while(vol > 30)
            {
                vol--;
                apply_settings();
            }
            vol = oldvol;
        }

        ioctl(fd_dsp,AV_DSP_PAUSE_MP3,NULL);
        pause=1;
    }
    main_drawn = 0; /* update window */
}

/**********************
 * Draw an oscillograph
 *********************/
void oscillograph(int l, int r)
{
    /* lower peak values */
    l = l/12;
    r = r/12;

    /* make sure the values aren't too high */
    if(l > 16)
        l = 16;
    if(r > 16)
        r = 16;

    /* keep to the right if not scrolling */
    if(scroll_osci == 1)
        osci_x = 268;

    /* draw a "cursor */
    if(scroll_osci == 0)
        lcd_drawline(COLOR_BLACK, osci_x+1, 208, osci_x+1, 240);

    /* clear trails */
    if(scroll_osci == 0)
        lcd_drawline(colortable[osci_bgcolor], osci_x, 208, osci_x, 240);

    /* left */
    lcd_drawline(colortable[osci_levelcolor], osci_x, 224, osci_x, 224-l);
    /* right */
    lcd_drawline(colortable[osci_levelcolor], osci_x, 224, osci_x, 224+r);

    /* move down a pixel, or back to the start */
    if(scroll_osci == 0)
    {
        if(osci_x < 270)
            osci_x++;
        else
            osci_x = 0;
    }
    /* stay in one spot, and just scroll */
    else if(scroll_osci == 1)
        scrollWindowHoriz(colortable[osci_bgcolor], 1, 208, 269, 32, 1, 0);
}

/***************************
 * Draw a set of peak meters
 **************************/
void peak_meters(int l, int r)
{
    /* left meter */
    lcd_fillrect(colortable[peak_bgcolor], lpos, 208, 270-lpos, 15);
    lcd_fillrect(colortable[peak_levelcolor], 0, 208, lpos, 10);

    /* right meter */
    lcd_fillrect(colortable[peak_bgcolor], rpos, 224, 270-rpos, 15);
    lcd_fillrect(colortable[peak_levelcolor], 0, 224, rpos, 10);
}

int main(int argc, char * * argv)
{
    if(argc<3)
       return 0; /* quit */
    else
    {
        filename=argv[1];
        vol=atoi(argv[2]);
    }
    printf("In soundTest\n");

    fd_dsp=open("/dev/dsp",O_WRONLY);
    if (fd_dsp < 0)
    {
        printf("Can't open /dev/dsp\n");
        return -1;
    }

    fd_mix=open("/dev/mixer",O_WRONLY);
    if (fd_mix < 0)
    {
        printf("Can't open /dev/mixer\n");
        return -1;
    }

    data.size=MP3_BUFF_SIZE;
    data.filename=filename;
    data.pos=0;
    data.finished=0;

    ioctl(fd_dsp,AV_DSP_INI_MP3,&data);

    wait=0;
    end=0;

    /* initialize the graphics and clear the lcd */
    ini_graphics();
    lcd_clear_display(COLOR_BLACK);

    /* set standard font */
    lcd_setfont(std6x9);

    /* start mp3 */
    ioctl(fd_dsp,AV_DSP_START_MP3,NULL);

    while(!data.finished)
    {
        while((evt=nxtEvent())>0)
        {
            /* Read the buttons */
            switch(evt)
            {
                case BUTTON_UP: /* settings_cursor up */
                    if(window == 1)
                    {
                        if(vol < 100)
                            vol++;
                        settings_applied = 0;
                        main_drawn = 0;
                    }
                    else if(window == 2)
                    {
                        if(settings_cursor_position > 1)
                            settings_cursor_position--;
                        else
                            settings_cursor_position = 8;

                        settings_drawn = 0;
                    }
                    else if(window == 3)
                    {
                        if(sound_cursor_position > 1)
                            sound_cursor_position--;
                        else
                            sound_cursor_position = 5;

                        soundsettings_drawn = 0;
                    }
                    break;

                case BUTTON_DOWN: /* settings_cursor down */
                    if(window == 1)
                    {
                        if(vol > 0)
                            vol--;
                        settings_applied = 0;
                        main_drawn = 0;
                    }
                    else if(window == 2)
                    {
                        if(settings_cursor_position < 8)
                            settings_cursor_position++;
                        else
                            settings_cursor_position = 1;

                        settings_drawn = 0;
                    }
                    else if(window == 3)
                    {
                        if(sound_cursor_position < 5)
                            sound_cursor_position++;
                        else
                            sound_cursor_position = 1;

                        soundsettings_drawn = 0;
                    }
                    break;

                case BUTTON_RIGHT: /* adjust up */
                    if(window == 2)
                    {
                        switch(settings_cursor_position)
                        {
                            case 1:
                                fade = !fade;
                                break;
                            case 2:
                                peakmeters = !peakmeters;
                                break;
                            case 3:
                                scroll_osci = !scroll_osci;
                                break;
                            case 4:
                                if(peak_decay < 10)
                                    peak_decay++;
                                break;
                            case 5:
                                if(peak_levelcolor < 9)
                                    peak_levelcolor++;
                                else
                                    peak_levelcolor = 0;
                                break;
                            case 6:
                                if(peak_bgcolor < 9)
                                    peak_bgcolor++;
                                else
                                    peak_bgcolor = 0;
                                break;
                            case 7:
                                if(osci_levelcolor < 9)
                                    osci_levelcolor++;
                                else
                                    osci_levelcolor = 0;
                                break;
                            case 8:
                                if(osci_bgcolor < 9)
                                    osci_bgcolor++;
                                else
                                    osci_bgcolor = 0;
                                break;
                        }
                        settings_drawn = 0;
                    }
                    else if(window == 3)
                    {
                        switch(sound_cursor_position)
                        {
                            case 1: if(vol < 100) vol++; break;
                            case 2: if(bass < 100) bass+=5; break;
                            case 3: if(treb < 100) treb+=5; break;
                            case 4: if(bal < 100) bal++; break;
                            case 5: if(loud < 100) loud+=5; break;
                        }
                        settings_applied = 0;
                        soundsettings_drawn = 0;
                    }
                    break;

                case BUTTON_LEFT: /* adjust down */
                    if(window == 2)
                    {
                        switch(settings_cursor_position)
                        {
                            case 1:
                                fade = !fade;
                                break;
                            case 2:
                                peakmeters = !peakmeters;
                                break;
                            case 3:
                                scroll_osci = !scroll_osci;
                                break;
                            case 4:
                                if(peak_decay > 0)
                                    peak_decay--;
                                break;
                            case 5:
                                if(peak_levelcolor > 0)
                                    peak_levelcolor--;
                                else
                                    peak_levelcolor = 9;
                                break;
                            case 6:
                                if(peak_bgcolor > 0)
                                    peak_bgcolor--;
                                else
                                    peak_bgcolor = 9;
                                break;
                            case 7:
                                if(osci_levelcolor > 0)
                                    osci_levelcolor--;
                                else
                                    osci_levelcolor = 9;
                                break;
                            case 8:
                                if(osci_bgcolor > 0)
                                    osci_bgcolor--;
                                else
                                    osci_bgcolor = 9;
                                break;
                        }
                        settings_drawn = 0;
                    }
                    else if(window == 3)
                    {
                        switch(sound_cursor_position)
                        {
                            case 1: if(vol > 0) vol--; break;
                            case 2: if(bass > 0) bass-=5; break;
                            case 3: if(treb > 0) treb-=5; break;
                            case 4: if(bal > 0) bal--; break;
                            case 5: if(loud > 0) loud-=5; break;
                        }
                        settings_applied = 0;
                        soundsettings_drawn = 0;
                    }
                    break;

                case BUTTON_ON: /* pause/resume */
                    if(window == 1)
                        pause_resume();
                    break;

                case BUTTON_OFF: /* quit */
                    if(window == 1)
                    {
                        if(fade)
                        {
                            while(vol > 35)
                            {
                                vol--;
                                apply_settings();
                            }
                        }
                        if(ioctl(fd_dsp,AV_DSP_STOP_MP3,NULL)<0)
                        {
                            printf("error stopping\n");
                            return -1;
                        }
                        goto end;
                    }
                    else if(window == 2 || window == 3)
                    {
                        lcd_clear_display(COLOR_BLACK); /* clear */
                        main_drawn = 0; /* redraw */
                        window = 1;
                    }
                    break;

                case BUTTON_F2: /* settings */
                    if(window == 1)
                    {
                        lcd_clear_display(COLOR_BLACK); /* clear */
                        settings_drawn = 0; /* redraw */
                        window = 2;
                    }
                    else if(window == 2)
                    {
                        lcd_clear_display(COLOR_BLACK); /* clear */
                        main_drawn = 0; /* redraw */
                        window = 1;
                    }
                    break;

                case BUTTON_F3: /* sound settings */
                    if(window == 1)
                    {
                        lcd_clear_display(COLOR_BLACK); /* clear */
                        soundsettings_drawn = 0; /* redraw */
                        window = 3;
                    }
                    else if(window == 3)
                    {
                        lcd_clear_display(COLOR_BLACK); /* clear */
                        main_drawn = 0; /* redraw */
                        window = 1;
                    }
                    break;
            }
        }

        /* main window */
        if(window == 1)
        {
            /* make sure the text is drawn */
            if(main_drawn == 0)
            {
                /* Print the version at the top */
                sprintf(tmp,"--= MP3 Player v1.10 =--");
                lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 65, 10, tmp);

                /* What's playing? */
                lcd_setfont(std7x13);
                sprintf(tmp,"Now Playing:");
                lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 40, tmp);
                sprintf(tmp,"%s",filename);
                lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 55, tmp);
                lcd_setfont(std6x9);

                /* fill in the peak meter background */
                if(peakmeters)
                {
                    lcd_fillrect(colortable[peak_bgcolor], 0, 208, 270-(av_p.left+10), 15);
                    lcd_fillrect(colortable[peak_bgcolor], 0, 224, 270-(av_p.left+10), 15);
                }

                draw_main_help_text();

                main_drawn = 1;
            }

            /* read peaks */
            ioctl(fd_dsp,AV_DSP_OUT_PEAK_REAL,&av_p);

            /* get peak values */
            av_p.left=(av_p.left*200)/0x7FFF;
            av_p.right=(av_p.right*200)/0x7FFF;

            /* smoothen out if desired */
            if(peak_decay > 0)
            {
                if(av_p.left < lpos)
                    lpos -= peak_decay;
                else
                    lpos = av_p.left;

                if(av_p.right < rpos)
                    rpos -= peak_decay;
                else
                    rpos = av_p.right;
            }
            else
            {
                lpos = av_p.left;
                rpos = av_p.right;
            }

            /* draw the peak meter, or the oscillograph */
            if(peakmeters)
                peak_meters(lpos, rpos);
            else
            {
                if(pause == 0)
                    oscillograph(av_p.left, av_p.right);
            }

            /* make sure the settings are applied */
            if(!settings_applied)
            {
                apply_settings();
                settings_applied = 1;
            }
        }
        else if(window == 2) /* settings */
        {
            /* make sure the text is drawn */
            if(settings_drawn == 0)
            {
                draw_settings();
                draw_settings_help_text();
                settings_drawn = 1;
            }

            /* make sure the settings are applied */
            if(settings_applied == 0)
            {
                apply_settings();
                settings_applied = 1;
            }
        }
        else if(window == 3) /* sound settings */
        {
            /* make sure the text is drawn */
            if(soundsettings_drawn == 0)
            {
                draw_soundsettings();
                draw_soundsettings_help_text();
                soundsettings_drawn = 1;
            }

            /* make sure the settings are applied */
            if(settings_applied == 0)
            {
                apply_settings();
                settings_applied = 1;
            }
        }
    }

    /* we're done */
    end:

    /* shut down everything used */
    close(fd_dsp);
    close(fd_mix);
    close_graphics();

    exit(0);
}
