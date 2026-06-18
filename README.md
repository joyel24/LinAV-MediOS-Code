"The goal of the project is to create an OS for archos device (Jbmm, Av and Gmini series). We are now working on our own kernel on: www.archopen.org. This kernel can be used as a lib to create seperate apps. It also has a gui that is able to launch elf li"
<br>https://sourceforge.net/projects/linav/

https://sourceforge.net/p/linav/news/

- Please consider using www.archopen.org to access news/forum ...
<br>Chris
<br>Posted by OxYgen77 2006-03-27

- New avlo release
<br>After a long time, a new release of avlo is now available, it is based on the same lib as mediOS.
This release also contains some changes in the look
Posted by OxYgen77 2005-12-16
GnuBoy for the AV available
The first stable version of the GameBoy emulator is now available for the av :)
This emu is based on bnuboy
http://sourceforge.net/project/showfiles.php?group_id=108103&package_id=171957&release_id=376647
<br>Posted by OxYgen77 2005-12-08

- New program: Gravity
<br>We have learned a lot during the process of porting linux on the av3xx series, and we have decided to use this knowkedge to build our own kernel: Gravity. Current code is multi-threaded and has drivers for the major parts of the hardware.
We should have something out for testing soon.
<br>Posted by OxYgen77 2005-04-13

- avlo part of linav
<br>Avlo is now part of linav, a package should soon be available.
<br>Posted by OxYgen77 2004-09-16

- New Gui
<br>We have released a new gui that uses our own graphical routines. See gui2 package.
<br>Posted by OxYgen77 2004-09-14

- gui-bin - linwin binaries
<br>New package released, it contains all binaries needed to launch linwin, our new gui. It also conatins avlo the new bootloader for the av3xx.
<br>Posted by OxYgen77 2004-08-11

- Current state
<br>Linav is now fully working, and can be used without any serial connexion. The first version of the sound driver can play all kind of mp3, next version will be able to move around the mp3, pause, stop ... Nano-x can be used as a X server, several apps can be displayed at the same time and window manager (nanowm) is available for displaying window's title, border and close box. The gui is launched automatically at the end of boot and you can use the joystick as a mouse cursor or as a simple input device.
A new bootloader (avlo) is available it's working a bit like lilo with a config file. This bootloader is part of avos project as it uses libavos.
Next steps should be some cleanup in the kernel, a new sound driver and a new frame buffer driver.
Of course help would be cool for the kernel or for coding apps using nano-x gui. Avlo also needs some more work so it includes more options in the config file.
<br>Posted by OxYgen77 2004-08-07

- cvs update
<br>now the kernel source and the nanox build env are on cvs
<br>Posted by OxYgen77 2004-08-06

- Nano-X build env.
<br>I've added a package with a basic build env for nano-X. This package can be used to build graphical apps using the nano-X API. The package contains a kernel with mouse support. And a system to automaticaly launch a file at the end of boot so you don't need a keyboard or a serial console to launch your apps
<br>Posted by OxYgen77 2004-07-28 

- 2 versions of linav released
<br>First release of the 2 version of linav:
<br>serial console => you'll need a serial cable to see something
<br>frame buffer => you'll see the boot process on av's screen, but you'll need a serial keyboard to have full access to the console
<br>Posted by OxYgen77 2004-07-22 

- First release
<br>This is the first patch release, the patch contains ide support, you'll need to download th uclinux source tree from linav.sf.net and the rom file.
<br>Posted by OxYgen77 2004-07-05 
