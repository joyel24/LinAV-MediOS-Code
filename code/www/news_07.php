<?
	$news->addItem("23-07-2004","Added a simple mouse driver for the joystick, also added the code needed so nano-X can use it. I'll release a new build env for nano-x today or tomorrow");
	$news->addItem("22-07-2004","Also released a package to build nano-X apps, this package contains a kernel and all configurations files to builds graphical applications. You don't need a keyboard nor a serial cablefor this.");
	$news->addItem("22-07-2004","First release of package on sf web site, there is a serial console and a frame buffer version of linav");
	$news->addItem("22-07-2004","New version of the mp3 player/ sound driver, now mp3 playback is interrupt driven. I will now concentrate on having a complete driver (with sound control), cleaning a bit the code and the config option of kernel compilation and having nano-X fully working.");
	$news->addItem("21-07-2004","First version of a mp3 player is working, the dsp driver is not interrupt driven, this adds some small pauses in the sound. This mean the mas driver is now working, I'll first finish the mp3 playback and then add the userland interface to change codec config (volume, bass, treble ...)");
	$news->addItem("20-07-2004","We now have a driver for the I2C bus, this give us access to the rtc (time/date), the real battery meter and the sound chip. Drivers are coded for rtc and battery, and they can be accessed from userland. I'm working on sound and on testing a first mp3 player");
	$news->addItem("06-07-2004","The framebuffer driver is fully working now, I've solved the problem of palette and of the one character dropping at the end of each line");
	$news->addItem("04-07-2004","I have port a frame buffer driver for the av, it can do 8bit colors for the moment");
	$news->addItem("01-07-2004","We have now the functions to display things on av's screen. uClinux's boot messages are now displayed on the screen");
?>