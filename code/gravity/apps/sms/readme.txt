GminiSMS v0.3
=============
by GliGli

GminiSMS is a Master System / GameGear emulator for the Gmini400.
It is based on SMS Plus by Charles Mac Donald.

/!\ Only works on 2.1.xx firmwares

Usage:
======

How to run the emulator:
  -Unzip this archive to a folder on your Gmini.
  -On your Gmini, go to this folder using the browser.
  -Play the .bin file, wait until the HDD led stops blinking 	then stop the file
    by pressing the X button.
  -Open the folder infos (F3 menu) of the folder corresponding to the language
    you chose in the Gmini settings.
  -The application should then start, if it does not and the Gmini freezes or
    crashes, try again until it works :).
    (If the Gmini freezes, you can shut it down by pressing OFF for 10 seconds)

File browser keys:
  -Up/Down: move in the file list
  -Left: up one level
  -Right/Square: open folder / load rom
  -Off: quit the emulator
                         
Ingame keys:         
  -On: Master System Pause button / GameGear Start button
  -F1: Options menu
  -F2: Reset console
  -F3+Up/Down/Left/Right: move Master System screen (Master System has a higher
    resolution than the Gmini400 screen)
  -Off: go to the file browser

Options menu:
=============

Frame skip options:	
  -Auto frame skip: frame skip is automaticaly chosen so that the game runs at 
     full speed (not perfect, manually choosing frame skip is better).
  -Frame skip: number of frames to skip between two rendered frames, if set
     too low, the game will not run at full speed.

Emulation options:
  -Sprite limit: MS/GG has a limit of 8 sprites per line, this enables it.
  -Expand GG screen: expand the viewable range of GameGear games to the full
    Gmini400 resolution, works well with some games, but will show only garbage
    with most games.
  -CPU cycles / line: Number of MS/GG CPU cycles executed for every line drawn.

Gmini overclocking:
	-Enable overclocking: Enables Gmini400 CPU overclocking, will help archieving 
		higher frame rates in games.
  -Overclocking rate: Lets you choose how much you want to overclock you Gmini,
    if set too high, it will certainely crash. Mine works well at 1 and 2, has
    some problems at 3 and crashes at 4 and 5.
    
  /!\ Use this option at your own risk, i'm not responsible if it damages your
	    Gmini. I think it's safe to use it, but you are warned.

History:
========

  -v0.3:
    First public release

What's next:
============

  -Options saving
  -Savestates support
  -Battery backed RAM cartriges support
  -Sound support

Credits:
========

  -Charles Mac Donald: SMS Plus author
  -DoggerMoore/awksedgrep/oxygen77: original AV300 libAVOS authors
  -mico/TechNIC: beta testers