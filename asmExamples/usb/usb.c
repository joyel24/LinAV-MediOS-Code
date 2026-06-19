// Author: Cheek
// Date: 12/02/2004

#include <usb.h>
#include <lcd.h>

int main() {
	lcdInitT();
	int usb;
	lcdPutsT (30, 10, "Cheex simple USB connect.", 110);

	while(usb!=1) {
		usb=usbIsConnected();
		lcdPutsT (30, 30, "Connect the USB to the AV.", 110);
	}
	usbEnable();
	while(usb==1) {
		usb=usbIsConnected();
		lcdPutsT (30, 50, "Transfer your files and disconnect.", 110);
	}
	usbDisable();
	lcdPutsT (30, 100, "Bye", 100);

	return 0;
}
