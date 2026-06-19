// buttons.h
#ifndef _BUTTONS_H_
#define _BUTTONS_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

#define  noButton 		0x777
#define  upButton		0x001
#define  leftButton		0x002
#define  rightButton	0x010
#define  downButton		0x020
#define  FastButton		0x400
#define  menu1Button	0x100
#define  menu2Button	0x200
#define  menu3Button	0x040

extern u32 buttonsGetStatusT();

int waitForKeyPress();
void waitForKeyReleased(int loopCount);


#endif
