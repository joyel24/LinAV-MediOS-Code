/*
* cops.c
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

#include "graphics.h"
#include "events.h"
#include "avwm.h"
#include "cops.h"
#include "avstring.h"
#include "avevents.h"
#include "plugin.h"
#include "sound.h"
#include "parse_cfg.h"
#include "msgBox.h"
#include "editBox.h"
#include "misc.h"
#include "avgraphics.h"
#include "debug.h"

#include <stdlib.h>

struct client_operations cops={
    openScreen         : ini_graphics,
    closeScreen        : close_graphics,
    
    drawPixel          : wmDrawPixel,
    readPixel          : wmReadPixel,
    drawRect           : wmDrawRect,
    fillRect           : wmFillRect,
    drawLine           : wmDrawLine,
    putS               : wmPutS,
    putC               : wmPutC,
    drawSprite         : wmDrawSprite,
    drawBITMAP         : wmDrawBITMAP,
    scrollWindowVert   : wmScrollWindowVert,
    scrollWindowHoriz  : wmScrollWindowHoriz,
    clearScreen        : wmClearScreen,
    
    drawImage          : drawImage,
    
    getStringS         : wmgetStringS,
    
    clearEventQueue    : clearEventQueue,
    registerPlugin     : registerPlugin,
    pack               : pack,
    stop_me            : stop_me,
    release_app        : myRelease_app,
    
    setFont            : wmSetFont,      
    
    setPlane           : wmSetPlane,
    getPlane           : wmGetPlane,
    hidePlane          : hidePlane,
    showPlane          : showPlane,
    cfgPlane           : cfgPlane,
    
    showSBar           : showSBar,
    hideSBar           : hideSBar,
    sBarStatus         : sBarStatus,
    
    enableMenu         : enableMenu,
    disableMenu        : disableMenu,
    menuStatus         : menuStatus,
    
    ini_mp3_playback   : ini_mp3_playback,
    close_mp3_playback : close_mp3_playback,
    start_playback     : start_playback,
    pause_playback     : pause_playback,
    stop_playback      : stop_playback,
    
    readPeak          : readPeak,
    
    setVolume          : setVolume,
    setBass            : setBass,
    setTreble          : setTreble,
    setLoudness        : setLoudness,
    setBalance         : setBalance,
    
    getVolume          : getVolume,
    getBass            : getBass,
    getTreble          : getTreble,
    getLoudness        : getLoudness,
    getBalance         : getBalance,
    
    debug              : debug,
    
    playMp3            : playMp3,
    
    openCfg            : openFile,
    closeCfg           : closeFile,
    getCfg             : nxt_cfg,
    putCfg             : write_cfg,
    putComment         : write_comment,
    newLine            : add_line,
    curLineNum         : curLineNum,
    
    msgBox             : msgBox,
    editBox            : editBox,

    getTick            : getTick,
    getTime            : getTime,
    getTimeS           : getTimeS,

    set_mouseParam     : set_mouseParam,
    get_mouseFreq      : get_mouseFreq,
    get_mouseRepeat    : get_mouseRepeat,

    stop_menu          : stop_menu,
    start_menu         : start_menu,
    menuEvtHandler     : menuEvtHandler,
    
    print_data         : print_data

};
