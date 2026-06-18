"The first stable version of the GameBoy emulator is now available for the av :)
This emu is based on bnuboy"

2005-12-08 https://sourceforge.net/p/linav/news/

#####################
# AvBoy version 1.0 #
#####################

   AvBoy is a port of gnuboy for AV3XX. It uses MediOS to work. You have to create 2 directories : /avboy/roms and /avboy/states. Put some roms in the roms directory, then start avboy.bin (with avlo for example).


In game :
#########

Joysick -> Directions
F1 -> B
F2 -> A
F3 -> Select
ON -> Start
OFF -> In-game menu


In a menu :
###########

UP,DOWN -> UP,DOWN
RIGHT -> Enter

In the in-game menu, you can :
  - Go back to the game
  - Save the current state of the game in a .avb (avboy will save the state in /states. There are 5 slots for each game). Save a state in a slot already used will overwrite it.
  - Load a state.
  - Access the Option menu (there is no option yet)
  - Quit AvBoy

IMPORTANT :
-----------

You have to use the Quit AvBoy option THEN shut the AV3XX down if you want avboy to save the SRAM data of the game.


To do :
#######

- Sound emulation
- A better speed
