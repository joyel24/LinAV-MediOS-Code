#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"
#define MWINCLUDECOLORS
#include "graphics.h"
#include "events.h"
#include "alias.h"

/***********************
 * Miscellaneous DEFINEs
 **********************/
#define MP3_BUFF_SIZE (1020*1000)
#define COLOR_WHITE    16
#define COLOR_BLUE     86
#define COLOR_DARKBLUE 55
#define COLOR_BLACK    1
#define COLOR_RED      13

/************
 * Fonts used
 ***********/
needFont(std6x9);
needFont(std7x13);

/*************************
 * Ints for PLAYBACK/SOUND
 ************************/
int vol, bass = 50, treb = 50; // sound settings
int bal = 50, loud = 0;        // sound settings
int oldvol;                    // used for pause/resume
int wait, end;
int fd_dsp, fd_file, fd_mix;
int pause = 0;                 // 1 if paused
int repeat;                    // repeat song?
int settings_applied = 0;      // "1" if settings have been applied
int frame_cnt;                 // frames elapsed so far
int evt;                       // button reading
int fade = 1;                  // fade on stop/pause

/*******************
 * Ints for GRAPHICS
 ******************/
int visualization = 1; // "1" for peak meters, "2" for oscillograph
int osci_x = 10;       // x position of oscillograph

/******************
 * Ints for WINDOWS
 *****************/
int window = 1;                   // "1" = main, "2" = settings, "3" = sound
int sound_cursor_position = 1;    // cursor position at sound settings
int settings_cursor_position = 1; // cursor position at settings
int main_drawn = 0;               // "1" if has been drawn
int settings_drawn = 0;           // "1" if has been drawn
int soundsettings_drawn = 0;      // "1" if has been drawn

/*********************
 * Miscellaneous CHARs
 ********************/
char defFilename[]="/mnt/file.mp3"; // default file location
char * filename;                    // current file location
char tmp[100];                      // used for printing text

/*******************
 * Other
 ******************/
struct mp3_play data; // mp3 data
struct av_peak av_p;  // left/right levels

void usage(void)
{
    printf("MP3 player v0.1 by oxygen77\n");
    printf("20/07/2004\n");
    printf("usage: play file\n");
}

/***************************************
 * Draws a progress bar on a 0-100 scale
 **************************************/
void draw_settings_progressbar(int x, int y, int value)
{
    lcd_drawrect(COLOR_WHITE, x, y, 104, 9);
    lcd_fillrect(COLOR_BLACK, x+2+value, y+2, 100-value, 5);
    lcd_fillrect(COLOR_BLUE, x+2, y+2, value, 5);
}

/**************************
 * Draw sound settings text
 *************************/
void draw_soundsettings(void)
{
    sprintf(tmp,"Vol: %03d",vol);
    if(sound_cursor_position == 1)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 100, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 100, tmp);

    sprintf(tmp,"Bass: %03d",bass);
    if(sound_cursor_position == 2)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 115, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 115, tmp);

    sprintf(tmp,"Treble: %03d",treb);
    if(sound_cursor_position == 3)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 130, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 130, tmp);

    sprintf(tmp,"Balance: %03d",bal);
    if(sound_cursor_position == 4)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 145, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 145, tmp);

    sprintf(tmp,"Loudness: %03d",loud);
    if(sound_cursor_position == 5)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 160, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 160, tmp);

    draw_settings_progressbar(100, 99, vol);
    draw_settings_progressbar(100, 114, bass);
    draw_settings_progressbar(100, 129, treb);
    draw_settings_progressbar(100, 144, bal);
    draw_settings_progressbar(100, 159, loud);
}

/**************************
 * Draw sound settings text
 *************************/
void draw_settings(void)
{
    if(visualization == 1)
        sprintf(tmp, "Visualization: Peak Meters ");
    else
        sprintf(tmp, "Visualization: Oscillograph");
    if(settings_cursor_position == 1)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 100, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 100, tmp);

    if(fade == 1)
        sprintf(tmp, "Fade On Stop/Pause: Yes");
    else
        sprintf(tmp, "Fade On Stop/Pause: No ");
    if(settings_cursor_position == 2)
        lcd_putsxy(COLOR_RED, COLOR_BLACK, 10, 115, tmp);
    else
        lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 115, tmp);
}

/********************
 * Apply the settings
 *******************/
void apply_settings(void)
{
    // apply volume
    ioctl(fd_mix,AV_SET_MIX_VOLUME,&vol);
    // apply bass
    ioctl(fd_mix,AV_SET_MIX_BASS,&bass);
    // apply treble
    ioctl(fd_mix,AV_SET_MIX_TREBLE,&treb);
    // apply balance
    ioctl(fd_mix,AV_SET_MIX_BALANCE,&bal);
    // apply loudness
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

    //lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 160, "Help");
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
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 10, "Use UP and DOWN to move the cursor.");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 25, "Use LEFT and RIGHT to change the value.");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 40, "Press OFF or F3 to exit.");

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
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 10, "Use UP and DOWN to move the cursor.");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 25, "Use LEFT and RIGHT to change the value.");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 40, "Press OFF or F3 to exit.");

    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275,  41, "Back");
    lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 275, 211, "Back");

    lcd_drawrect(COLOR_WHITE, 272,  14, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272,  39, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 149, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 179, (320-272), 13);
    lcd_drawrect(COLOR_WHITE, 272, 209, (320-272), 13);
}

/**********************
 * Draw an oscillograph
 *********************/
void oscillograph(int l, int r)
{
    // lower the values
    l = l/12;
    r = r/12;

    // make sure the values aren't too high
    if(l > 16)
        l = 16;
    if(r > 16)
        r = 16;

    // draw a "cursor"
    lcd_drawline(COLOR_BLACK, osci_x+1, 208, osci_x+1, 240);

    // left
    lcd_drawline(COLOR_DARKBLUE, osci_x, 224, osci_x, 208);
    lcd_drawline(COLOR_BLUE, osci_x, 224, osci_x, 224-l);

    // right
    lcd_drawline(COLOR_DARKBLUE, osci_x, 224, osci_x, 240);
    lcd_drawline(COLOR_BLUE, osci_x, 224, osci_x, 224+r);

    // move down a pixel, or back to the start
    if(osci_x < 270)
        osci_x++;
    else
        osci_x = 10;
}

int main(int argc, char * * argv)
{
    printf("[play] argc: %d\n", argc);

    if(argc<3)
       return 0; // Quit
    else
    {
        filename=argv[1];
        vol=atoi(argv[2]);
        repeat=atoi(argv[3]);
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

    printf("Ready to play\n");

    wait=0;
    end=0;
    frame_cnt=0;

    if(ini_graphics()<0)
    {
        printf("Cannot open graphics\n");
        exit(1);
    }

    // black out the display
    lcd_clear_display(COLOR_BLACK);

    // set standard font
    lcd_setfont(std6x9);

    ioctl(fd_dsp,AV_DSP_START_MP3,NULL);

    while(!data.finished)
    {
        while((evt=nxtEvent())>0)
        {
            switch(evt)
            {
                case BUTTON_UP: // settings_cursor up
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
                            settings_cursor_position = 2;

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

                case BUTTON_DOWN: // settings_cursor down
                    if(window == 1)
                    {
                        if(vol > 0)
                            vol--;
                        settings_applied = 0;
                        main_drawn = 0;
                    }
                    else if(window == 2)
                    {
                        if(settings_cursor_position < 2)
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

                case BUTTON_RIGHT: // adjust up
                    if(window == 2)
                    {
                        switch(settings_cursor_position)
                        {
                            case 1:
                                if(visualization == 1)
                                    visualization = 2;
                                else
                                    visualization = 1;
                                break;

                            case 2:
                                if(fade == 1)
                                    fade = 0;
                                else
                                    fade = 1;
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

                case BUTTON_LEFT: // adjust down
                    if(window == 2)
                    {
                        switch(settings_cursor_position)
                        {
                            case 1:
                                if(visualization == 2)
                                    visualization = 1;
                                else
                                    visualization = 2;
                                break;

                            case 2:
                                if(fade == 1)
                                    fade = 0;
                                else
                                    fade = 1;
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

                case BUTTON_ON: // pause/resume
                    if(window == 1)
                    {
                        if(pause) // resume
                        {
                            if(ioctl(fd_dsp,AV_DSP_START_MP3,NULL)<0)
                            {
                                printf("Error resuming\n");
                                return -1;
                            }
                            pause=0;

                            // fade in
                            if(fade)
                            {
                                vol = 35;
                                while(vol < oldvol)
                                {
                                    vol++;
                                    apply_settings();
                                }
                            }
                        }
                        else // pause
                        {
                            // fade out
                            if(fade)
                            {
                                oldvol = vol;
                                while(vol > 35)
                                {
                                    vol--;
                                    apply_settings();
                                }
                                vol = oldvol;
                            }

                            if(ioctl(fd_dsp,AV_DSP_PAUSE_MP3,NULL)<0)
                            {
                                printf("Error pausing\n");
                                return -1;
                            }
                            pause=1;
                        }
                        main_drawn = 0; // update window
                    }
                    break;

                case BUTTON_OFF: // quit
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
                        lcd_clear_display(COLOR_BLACK); // clear
                        main_drawn = 0; // redraw
                        window = 1;
                    }
                    break;

                case BUTTON_F2: // settings
                    if(window == 1) // switch to settings
                    {
                        lcd_clear_display(COLOR_BLACK); // clear
                        settings_drawn = 0; // redraw
                        window = 2;
                    }
                    else
                    {
                        lcd_clear_display(COLOR_BLACK); // clear
                        main_drawn = 0; // redraw
                        window = 1;
                    }
                    break;

                case BUTTON_F3: // sound settings
                    if(window == 1) // switch to settings
                    {
                        lcd_clear_display(COLOR_BLACK); // clear
                        soundsettings_drawn = 0; // redraw
                        window = 3;
                    }
                    else
                    {
                        lcd_clear_display(COLOR_BLACK); // clear
                        main_drawn = 0; // redraw
                        window = 1;
                    }
                    break;
            }
        }

        // main window
        if(window == 1)
        {
            // read peaks
            ioctl(fd_dsp,AV_DSP_OUT_PEAK_REAL,&av_p);

            // get peak values
            av_p.left=(av_p.left*200)/0x7FFF;
            av_p.right=(av_p.right*200)/0x7FFF;

            // draw a peak meter or an oscillograph
            if(visualization == 1)
            {
                // left meter
                lcd_fillrect(COLOR_DARKBLUE, av_p.left+10, 208, 270-(av_p.left+10), 15);
                lcd_fillrect(COLOR_BLUE, 10, 208, av_p.left, 15);

                // right meter
                lcd_fillrect(COLOR_DARKBLUE, 10+av_p.right, 224, 270-(av_p.right+10), 15);
                lcd_fillrect(COLOR_BLUE, 10, 224, av_p.right, 15);
            }
            else if(visualization == 2)
            {
                if(pause == 0)
                    oscillograph(av_p.left, av_p.right);
            }

            // make sure the text is drawn
            if(main_drawn == 0)
            {
                // Print the version at the top
                sprintf(tmp,"--= MP3 Player v0.40 =--");
                lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 65, 10, tmp);

                // "Now Playing: X.mp3"
                lcd_setfont(std7x13);
                sprintf(tmp,"Now Playing:");
                lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 40, tmp);
                sprintf(tmp,"%s",filename);
                lcd_putsxy(COLOR_WHITE, COLOR_BLACK, 10, 55, tmp);
                lcd_setfont(std6x9);

                draw_main_help_text();

                main_drawn = 1;
            }

            // make sure the settings are applied
            if(settings_applied == 0)
            {
                apply_settings();
                settings_applied = 1;
            }
        }
        else if(window == 2) // settings
        {
            // make sure the text is drawn
            if(settings_drawn == 0)
            {
                draw_settings();
                draw_settings_help_text();
                settings_drawn = 1;
            }

            // make sure the settings are applied
            if(settings_applied == 0)
            {
                apply_settings();
                settings_applied = 1;
            }
        }
        else if(window == 3) // sound settings
        {
            // make sure the text is drawn
            if(soundsettings_drawn == 0)
            {
                draw_soundsettings();
                draw_soundsettings_help_text();
                soundsettings_drawn = 1;
            }

            // make sure the settings are applied
            if(settings_applied == 0)
            {
                apply_settings();
                settings_applied = 1;
            }
        }
    }

    end:

    close(fd_dsp);
    close(fd_mix);
    printf("mixer closed\n");

    close_graphics();

    printf("I'm out \n");

    exit(0);
}
