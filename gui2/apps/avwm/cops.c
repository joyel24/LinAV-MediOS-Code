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

#include <stdlib.h>

struct client_operations cops={
    drawPixel          : drawPixel,
    readPixel          : readPixel,
    drawRect           : drawRect,
    fillRect           : fillRect,
    drawLine           : drawLine,
    putS               : wmPutS,
    putC               : wmPutC,
    getStringS         : wmgetStringS,
    drawSprite         : drawSprite,
    drawBITMAP         : drawBITMAP,
    scrollWindowVert   : scrollWindowVert,
    scrollWindowHoriz  : scrollWindowHoriz,
    clearEventQueue    : clearEventQueue,
    registerPlugin     : registerPlugin,
    pack               : pack,
    stop_me            : stop_me,
    release_app        : myRelease_app,
    drawImage          : drawImage,
    openScreen         : ini_graphics,
    closeScreen        : close_graphics,
    setFont            : wmSetFont,
    showSBar           : showSBar,
    hideSBar           : hideSBar,
    sBarStatus         : sBarStatus,    
    clearScreen        : clearScreen,
    setPlane           : setPlane,
    hidePlane          : hidePlane,
    showPlane          : showPlane,
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
    
    execDSP_ioctl      : execDSP_ioctl,
    execMIX_ioctl      : execMIX_ioctl,
    
    debug              : debug,
    
    playMp3            : playMp3,
    
    openCfg            : openFile,
    closeCfg           : closeFile,
    getCfg             : nxt_cfg,
    putCfg             : write_cfg,
    putComment         : write_comment,
    newLine            : add_line,
    curLineNum         : curLineNum,
    
    msgBox             : msgBox

};
