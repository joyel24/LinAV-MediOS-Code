#include "cops.h"
#include "colordef.h"

/* client graphical ops */
extern struct client_operations * cops;

/*extern variables */
extern int vol;
extern int pause;
/*******************/

/**************************
 * Draw text along the side
 *************************/
void draw_main_help_text(void)
{
    char tmp[60];
    if(pause == 0)
    {
        cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 16, "Pause    ");
    }
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 16, "Resume");

    cops->putS(COLOR_WHITE, COLOR_BLACK, 275,  41, "Stop");

    sprintf(tmp,"Vol:%03d",vol);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 275,  96, tmp);

    /* cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 160, "Help"); */
    cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 181, "Options");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 211, "Sound");

    cops->drawRect(COLOR_WHITE, 272,  14, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272,  39, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272,  94, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 149, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 179, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 209, (320-272), 13);
}

/**************************
 * Draw text along the side
 *************************/
void draw_settings_help_text(void)
{
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 0,
               "UP/DOWN: Move cursor. LEFT/RIGHT: Change value.");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 10, "OFF/F2: Exit");

    cops->putS(COLOR_WHITE, COLOR_BLACK, 275,  41, "Back");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 181, "Back");

    cops->drawRect(COLOR_WHITE, 272,  14, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272,  39, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 149, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 179, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 209, (320-272), 13);
}

/**************************
 * Draw text along the side
 *************************/
void draw_soundsettings_help_text(void)
{
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 0,
               "UP/DOWN: Move cursor. LEFT/RIGHT: Change value.");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 10, "OFF/F3: Exit");

    cops->putS(COLOR_WHITE, COLOR_BLACK, 275,  41, "Back");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 211, "Back");

    cops->drawRect(COLOR_WHITE, 272,  14, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272,  39, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 149, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 179, (320-272), 13);
    cops->drawRect(COLOR_WHITE, 272, 209, (320-272), 13);
}
