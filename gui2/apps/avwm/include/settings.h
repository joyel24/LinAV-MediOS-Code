/*
* settings.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Goetz Minuth
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __SETTINGS_H
#define __SETTINGS_H

int  GetActiveSetting    (void);
void SettingsEvtLoop     (void);
void GetSettings         (void);
void SetSettings         (void);
int  settingsEvtHandler  (int evt);
void drawSlider          (int x, int y, char* text, int min, int max, int value, int active);
void drawSettings        (void);
void ini_settings        (void);
void eraseSettings       (void);
int  SettingsScreen      (void);

#endif
