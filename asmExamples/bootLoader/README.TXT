avOS Bootloader Version 0.3a                    (c)2004 Jimmy Moore (DoGgEr)
----------------------------

What does it do?
================
    The avos bootloader is replacement firmware for the av300. It loads when the
machine is started, and presents the user with a list of programs to run. The
user can choose a program, and run it.

What does it not do?
====================
    It does NOT support packed files (Such as the official firmware AJZ file).
So to use the official firmware, you must unpack it yourself first.

Has it been tested thoroughly by an army of testers?
====================================================
    So far it has been tested by me, and Oxygen. There don't seem to be any
critical bugs. If for some reason it did render your box unusable, there is
always a serial recovery mode which can be used to restore the firmware, if
you have a serial cable.

Can I uninstall the bootloader and go back to the official firmware?
====================================================================
    Of course you can. IF you have the official firmware in binary unpacked form
then it's just a case of putting the official cjbm.ajz file on your archos,
booting the bootloader, running the official firmware unpacked file, then
select update firmware. This will restore the official firmware.
DO NOT TRY TO RUN THE CJBM.AJZ FILE FROM BOOTLOADER.

So how does it know whats an executable?
========================================
    It doesn't. It trusts you. If you choose to run a picture, your av300 will
most likely just crash, and maybe turn off.

What are the keys in the bootloader?
====================================
    Use up and down to move up and down in the file/dir list. Use menu1 to enter
a directory, menu2 to run a program.

So what can we run so far?
==========================
    So far, there are a few little demo programs written (Checkout asmExamples
in cvs). etch is quite a nice etch-a-sketch.

What do I need to develop for this thing then?
==============================================
    First off, you need to have an IQ of >100. Then get gnu binutils toolchain
and build it for arm. Then you could play with garme (A cool arm emulator)
www.sf.net/projects/garme.
    If you want to go hardcore, you might wanna get a serial cable. I use a
nokia 3210 data cable, with the plug changed. Also get a keyboard. I use a
handspring visor foldup PDA keyboard. This is the only one I know of with the
right levels/connections.

Where can I get help?
=====================
    Load up an IRC client, connect to irc.freenode.net, and join the #avos
channel. I am there most days. Please make sure to ask well formed and thought
out questions though :)

You broke my av300! You owe me $1,000,000
=========================================
    No I didn't. YOU broke your av300. Anything you do with your hardware is
totally your problem. I cannot be held responsible for ANY bad things that may
happen to you or your archos by using this software.
