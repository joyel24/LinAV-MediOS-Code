Please read before using:
============================

INSTALL:
=========

To install create a folder called "ABook" in the root of your AV3xx and put all files(bin, csv and ini) in it.

START:
=======

This is version 1.3 of my address book for the AV3xx. It´s based on a csv data file.
It simply shows all entries from the file in an overliew list or an full entry list.

With Version V1.3 and above there is a birthday reminder at start up which shows all people in the list
how have birthday today.



Each line of the csv-file must have the following format:

Name;First Name;Street;PLZ;Residence;Country;Phone;Handy;Mail;Birthday

At the end must be an "\n"(return).

As you see a birthday element is added. So please add this to existing older csv-files.

The csv ist named "address.csv". The entrys of the csv-file are sorted by name at start up time automatically.



In version 1.2 and above you can customize all colors and texts as you like in an ini file called abook.ini .
Most colors are from the internal palette and some are rgb-values. To see the hex-codes of the
internal colors use the paltest.bin programm(look in the bin section of avos site).

Remark Version V1.3: A new text is added to the abook.ini for the Brirthday Reminder. Please add this new text at
the end of older ini-files (compare with the ini-files in the new archive).

If the file abook.ini is missing i use default values. I put an english and german example to the
archive. Just rename the file you want to use to abook.ini and place it in the abook-folder.

Keys:
==============

In List View
--------------

On   --- Toggle between the normal list view and the birthday list
Off  --- Quit
F1   --- New (create anew entry)
F2   --- Delete ( deletes the current marked entry in the list )
F2   --- Show ( Shows the marked entry with all elements)
Up   --- marks previous entry 
Down --- marks next entry
Left --- go one page back (15 entrys back)
Right--- go one page forward (15 entrys forward)

In the status the number of entrys and the actual range.

Show Entry:
--------------

Left  --- shows previous entry 
Right --- shows next entry
F1    --- Ok (return to List view)
F2    --- Delete ( deletes this entry )
F3    --- Edit ( you can edit all elements of the entry )

In the status line at the bottom you see witch entry is shown and the whole number of entrys.

Edit Entry:
--------------

Off  --- Delete a char at the current position of the line
F1   --- Ok (return to Show Entry with saving)
F2   --- Cancel (return to Show Entry without saving changes)
F3   --- Next ( sets the cursor in the next element to edit )
Up   --- select Ascii-Code forward
Down --- select Ascii-Code back
Left --- go one char forward
Right--- go one char back forward

The actual position of the cursor is underlined.
In the status line at the bottom you see witch entry is shown and the whole number of entrys.

New Entry:
--------------

Same keys and behaviour as Edit Entry.

Messages Boxes:
---------------

To answer press the named F1 - F3 keys on the right.



ToDo:
==============

- Maybe same more entry fields ( Comment, third phone number...)
- Search
- Sort on different elements

Known bugs:
==============

- sometimes you must start the programm two times before running, i dont know why
- if you start and there is no csv-file you get an error message. But everything is still ok, just
  press a key and go on.
- if you quit the first time the saving is (very) slow, so please wait a moment. Sometimes the program
  never ends. I think it must be a bug in the fopen-function from AVOS-lib (it never returns out of this function)

History:
==============

12.06.04 version 1.31 - Bug fixed when use Key to the right in list mode
                      - birthday list is now sorted first to month and second to day

06.06.04 version 1.3  - added birthday list (Toogle with "on"-Button in List Mode)
                      - added birthday reminder at start up
                      - added actual date in the head line

03.06.04 version 1.23 - Bug fixed while saving. The saved file was created in the root.

02.06.04 version 1.22 - Bug fixed when writing changes. A new file was created in the root instead of 
                        writing the changes in the csv-File in the abook-directory.

26.05.04 version 1.21 - ABook has now its own directory called abook with all files in it.
                      - bug in show entry mode while selecting next entry fixed
                      - some very little design changes

25.05.04 version 1.2  - added ini file for customize colors and texts
                      - added support for extended characters in new mode
                      - bug in New-Modus fixed(cursor on incorrect postion)

08.05.04 version 1.01 - i have changed the ascii order when selecting the chars with up and down keys so
                        you have easier access to the most important ones.
                      - when you use the right key when editing and you are at the end of the word, the new
                        char will be the same as the last one by default(and not always an 'A').

07.05.04 version 1.00 - added editing
                      - added saving
                      - its now possible to create new entrys
                      - message boxes
                      - many design changes
                      - automatically sorting at start up
                      - added status line
                      - added bithday element
                         
23.04.04 version 0.21 small improvement(Left+Right in detail view added) and a bug
                      removed (the last entry on a page was not shown).

22.04.04 Version 0.2  FirstRelease


If you have wishes for next versions or you found an error please let me know:

minuth@gmx.de


Greets

Goetz

