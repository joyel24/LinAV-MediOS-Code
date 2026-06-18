<?
	$news->addItem("27-08-2004","I've added a real menu to avwm, the menu layout is saved in a text file.
<br> Schocki has coded a calendar plugin, and midk's mp3 player should be out soon.");
	$news->addItem("26-08-2004","A reference to linav project has been made in several news website and also in the french version of linux magazine");
	$news->addItem("25-08-2004","I've succesfully build libjpeg and customized it a bit so it can use av's display. We now have a jpeg viewer that should work for all kind of jpeg size. I'm not sure we will keep this, as it is rather slow. But it can be a good starting point.");
	$news->addItem("25-08-2004","The first version of the new gfx is working, we are now porting some apps to it, we have also added a plugin system and a framework for a gui.");
	$news->addItem("19-08-2004","After a lot of testing on nano-x, we've come to the conclusion it will be too slow
to be used on the av3xx. So, we've decided to create our own graphical lib. I've finished the first version, and it has all the features
needed to build our mp3 player. The progs build with this new lib are still compatible with the one made with nano-x
so we will keep linwin as a browser for the moment.");
	$news->addItem("12-08-2004","New package released on www.sf.net. It provides all the binaries needed to run linwin, linav's gui!! 
(<a href='http://sourceforge.net/project/showfiles.php?group_id=108103&package_id=126697'>download</a>)");
	$news->addItem("07-08-2004","<b>First news of august shall big news :)</b><br>
Linav is now fully working, and can be used without any serial connexion. 
The first version of the sound driver can play all kind of mp3, next version will be able to move around the mp3, pause, stop ... 
Nano-x can be used as a X server, several apps can be displayed at the same time and window manager (nanowm) is available for displaying window's title, border and close box. 
The gui is launched automatically at the end of boot and you can use the joystick as a mouse cursor or as a simple input device. <br>
A new bootloader (avlo) is available it's working a bit like lilo with a config file. 
This bootloader is part of avos project as it uses libavos.<br>
Next steps should be some cleanup in the kernel, a new sound driver and a new frame buffer driver.<br>
Of course help would be cool for the kernel or for coding apps using nano-x gui. Avlo also needs some more work so it includes more options in the config file.");
	
?>
