/*
* cops.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __COPS_H
#define __COPS_H

#include "graphics.h"
#include "parse_cfg.h"
#include "misc.h"
#include "sound.h"

#define REGISTER(ops,event,flag)    {ops=(struct client_operations*)atoi(argv[argc-1]);ops->registerPlugin(event,flag);}
#define PACK(ops,loop)              {ops->pack(loop);}
#ifdef AV_SCREEN
    #define RELEASE(ops)                {ops->release_app();}
    #define STOPME(ops)                 {ops->stop_me();}
#else
    #define RELEASE(ops)                {ops->stop_me();}
    #define STOPME(ops)                 {;}
#endif


#define MENU_SHADOW 2 // height of small shadow under the title

struct client_operations { 
   
    void (*closeScreen)       (void);
    int  (*openScreen)        (void);
    void (*clearScreen)       (unsigned int color);

    void (*drawPixel)         (unsigned int color, int x, int y);
    unsigned int  (*readPixel)         (int x, int y);
    void (*drawRect)          (unsigned int color, int x, int y, int width, int height);
    void (*fillRect)          (unsigned int color, int x, int y, int width, int height);
    void (*drawLine)          (unsigned int color, int x1, int y1, int x2, int y2);
    
    void (*getStringS)        (unsigned char *str, int *w, int *h);
    void (*putS)              (unsigned int color, unsigned int bg_color,int x, int y, unsigned char *s);
    void (*putC)              (unsigned int color, unsigned int bg_color,int x, int y, unsigned char s);
    void (*setFont)           (int font);
    
    void (*drawSprite)        (PALETTE * palette, SPRITE * sprite, int x, int y);
    void (*drawBITMAP)        (BITMAP * bitmap, int x, int y);
    void (*drawImage)         (char * filename);
    
    void (*scrollWindowVert)  (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP);
    void (*scrollWindowHoriz) (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);
    
    
    int  (*clearEventQueue)   (void);    
    void (*registerPlugin)    (void (*evtHandle),int flag);    
    void (*pack)              (void (*loopFct)(void));
    void (*stop_me)           (void);    
    void (*release_app)       (void);
    
    void (*showSBar)          (void);
    void (*hideSBar)          (void);
    int  (*sBarStatus)        (void);
    
    void (*enableMenu)        (void);
    void (*disableMenu)       (void);
    int  (*menuStatus )       (void); 
    
    void (*setPlane)          (int vplane);
    void (*hidePlane)         (int vplane);
    void (*showPlane)         (int vplane);
    
    int  (*ini_mp3_playback)    (struct mp3_play * mp3_p_data);
    void (*close_mp3_playback)  (void);
    int  (*start_playback)      (void);
    int  (*pause_playback)      (void);
    int  (*stop_playback)       (void);
    
    int  (*readPeak)           (struct av_peak * peak); 
    
    int  (*setVolume)           (int val);
    int  (*setBass)             (int val);
    int  (*setTreble)           (int val);
    int  (*setLoudness)         (int val);
    int  (*setBalance)          (int val);
    
    int  (*getVolume)           (void);
    int  (*getBass)             (void);
    int  (*getTreble)           (void);
    int  (*getLoudness)         (void);
    int  (*getBalance)          (void);
    
    void (*debug)               (char *fmt, ...);
    
    int  (*playMp3)             (char * filename);
    
    int  (*openCfg)             (char * filename,int mode);
    void (*closeCfg)            (void);
    int  (*getCfg)              (char *item,char *value);
    int  (*putCfg)              (char * item,char * value);
    int  (*putComment)          (char * cmt);
    int  (*newLine)             (void);
    int  (*curLineNum)          (void);
    
    int  (*msgBox)              (unsigned char * msg);
    
    int  (*getTick)             (void);
    int  (*getTime)             (struct av_tm * date_time);
    int  (*getTimeS)            (char * timeSt);
    
};

#endif
