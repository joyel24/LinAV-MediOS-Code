#ifndef __AONES_GUI_H
#define __AONES_GUI_H

#include "medios.h"

#define GUI_TITLE_COLOR COLOR_RED

extern bool gui_browserNeedInit;
extern struct browser_data * browser;

void gui_init();
void gui_close();
void gui_execute();
void gui_applySettings();
bool gui_browse();
void gui_welcomeScreen();
bool gui_confirmQuit();
void gui_showGuiPlane();
void gui_showEmuPlane();
void gui_showText(char * text);

#endif
