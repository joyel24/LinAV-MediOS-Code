/*
*   kernel/target/arch_AV4XX/exit.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>
#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/irq.h>
#include <kernel/graphics.h>
#include <kernel/lcd.h>
#include <kernel/osd.h>
#include <kernel/clkc.h>
#include <kernel/exit.h>

typedef struct{
    unsigned char magic[4];
    int unpackedsize;
    int packedsize;
    int checksum;
} firmware_header;

// code taken from descramble.c from the Rockbox project
__attribute__((section(".fwuncomp_code"))) void firmware_decompress(unsigned char * inbuf,unsigned char * outbuf, int length){
    int i;
    int j=0;

    for (i=0; i<length;) {
        int bit;
        int head = inbuf[i++];

        for (bit=0; bit<8 && i<length; bit++) {
            if (head & (1 << (bit))) {
                outbuf[j++] = inbuf[i++];
            }
            else {
                int x;
                int byte1 = inbuf[i];
                int byte2 = inbuf[i+1];
                int count = (byte2 & 0x0f) + 3;
                int src =
                    (j & 0xfffff000) + (byte1 | ((byte2 & 0xf0)<<4)) + 18;
                if (src > j)
                    src -= 0x1000;

                for (x=0; x<count; x++)
                    outbuf[j++] = outbuf[src+x];
                i += 2;
            }
        }
    }
}

__attribute__((section(".fwuncomp_code"))) void arch_reload_firmware(void){
    void (*firmware_start)(void) = (void (*)(void)) SDRAM_START;
    firmware_header header=*(firmware_header*)0x120000;
    unsigned char * fwdata=(unsigned char *)0x120010;
    unsigned short * fwfb=(unsigned short *)0x18d1500; //official firmware bitmap plane address
    if(CPLD_VER==0x5)
    {
        printk("Reload not supported atm => halt\n");
        halt_device();
    }
    else
    {
        // disable interrupts
        cli();
   
        // show something on the screen (black screen for now)
        gfx_openGraphics();
    
        gfx_planeHide(BMAP1);
        gfx_planeHide(BMAP2);
        gfx_planeHide(VID1);
        gfx_planeHide(VID2);
        gfx_planeHide(CUR1);
        gfx_planeHide(CUR2);
    
        gfx_planeSetBufferOffset(BMAP1,fwfb);
        gfx_planeSetSize(BMAP1,240*2,176,8);
        gfx_planeSetState(BMAP1,OSD_BITMAP_8BIT);
    
        gfx_clearScreen(COLOR_ROM_BLACK);
        gfx_planeShow(BMAP1);
    
        // set default clock parameters (not reinitialized by the firmware)
        clkc_setClockParameters(CLK_ARM,15,2,2);
        clkc_setClockParameters(CLK_SDRAM,15,2,2);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
        clkc_setClockParameters(CLK_DSP,9,1,2);
        outw(0x40e0,CLKC_SOURCE_SELECT); //restore uart & timer source select
    
        // uncompressing firmware
        firmware_decompress(fwdata,(unsigned char *)SDRAM_START,header.packedsize);
    
        //jump to sdram start (where the uncompressed firmware is)
        firmware_start();
    }
}

void arch_HaltMsg(void)
{

}
