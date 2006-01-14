/*
*   arch-av3xx.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#define ARCH_NAME         "Gmini400"

//#define homebrew //or let it be ;-)

/************************************************************ Memory Zone */

#define RESET_VECTOR      0x00000000

#define IRAM_START        0x00000004
#define IRAM_END          0x00008000
#define TI_REG_START      0x00030000
#define TI_REG_END        0x00040000

/*IRAM should be the same on all arch */



#define FLASH_START       0x0100000
#define FLASH_END         0x0180000
#define FLASH_LOAD_OFFSET	0x0010000

#define SDRAM_START       0x0900000
#define SDRAM_END         0x1900000

#ifndef homebrew
#define SDRAM_LOAD_OFFSET	0x0000000 // original;
#else
#define SDRAM_LOAD_OFFSET	0x310000 //gmini400 homebrew;
#endif




/************************************************************ initial state */

#ifndef homebrew
#define START_ADDR        SDRAM_START //original
#else
#define START_ADDR        0xC10000  //gmini400 homebrew;
#endif

#define STACK_INIT        (IRAM_END - 0x4)
#define RESET_INIT_VAL    0xEA03FFFE
#define INIT_MODE         M_SVC

/* Special init for this arch: enable monitor command line mode */
#define ARCH_INIT   \
{                   \
    write(0x10600f,0x55,1);\
}

/************************************************************ HW config */

/********************** TIMERS    ****************************************/
#define TMR0_START   TI_REG_START+0x000
#define TMR1_START   TI_REG_START+0x080
#define TMR2_START   TI_REG_START+0x100
#define TMR3_START   TI_REG_START+0x180

/********************** UART 0   ****************************************/
#define UART0_START   TI_REG_START+0x300
#define UART0_END     TI_REG_START+0x310

/********************** UART 1   ****************************************/
#define UART1_START   TI_REG_START+0x380
#define UART1_END     TI_REG_START+0x390

/********************** WDT   ****************************************/
#define WDT_START   TI_REG_START+0x400
#define WDT_END     TI_REG_START+0x406
#define WDT_FIQ     0x1b
/********************** IRQ      ****************************************/
#define IRQ_START   TI_REG_START+0x500
#define IRQ_END     TI_REG_START+0x57f

#define FIQ_0    0x0
#define FIQ_1    0x2
#define FIQ_2    0x4
#define IRQ_0    0x8
#define IRQ_1    0xA
#define IRQ_2    0xC

#define BASE_STATUS   (IRQ_START+0x0)
#define BASE_ENTRY    (IRQ_START+0xE)
#define BASE_ENABLE   (IRQ_START+0x26)

#define NUM_OF_IRQ 7 //1 more (bogus) because of the missing 0x6 
#define REG_NUM(irq) (irq<16?0:irq<32?1:2)
#define REAL_NUM(irq) (irq<16?irq:irq<32?irq-16:irq-32) //get the irq line number in the reg

/********************** GPIO     ****************************************/
#define GPIO_START   TI_REG_START+0x580
#define GPIO_END     TI_REG_START+0x596

#define GPIO_STR  \
{ "ON" , "SPDIF/UART1_RX", "UKN", "UKN", "MAS_EOD", "UKN", "IR", "UKN", \
  "MAS_D0", "MAS_D1", "MAS_D2", "MAS_D3", "MAS_D4", "MAS_D5", "MAS_D6", "MAS_D7", \
                    \
  "MAS_PWR", "UKN", "I2C_CLK", "I2C_DA", "UKN", "CPLD_MOD_SENSE", "CPLD_SIGNAL", "BCK_LIGHT", \
  "UKN", "UKN", "UKN", "UKN", "OFF", "VIDEO/UART1_TX", "MAS_RTR", "MAS_PR"  \
} 

#define GPIO_ON_NUM   0x00
#define GPIO_MAS_EOD  0x04
#define GPIO_MAS_Di   0x08
#define GPIO_MAS_PW   0x10

#define GPIO_OFF_NUM  0x1C
#define GPIO_MAS_PR   0x1F

/********************** I2C     ****************************************/

/********************** OSD     ****************************************/
#define OSD_START   TI_REG_START+0x680
#define OSD_END     TI_REG_START+0x700
//#define OSD_START   0x18d0300
//#define OSD_END     0x18d0320

/********************** CCD     ****************************************/
#define CCD_START   TI_REG_START+0x700
#define CCD_END     TI_REG_START+0x73A

/********************** PREVIEW ****************************************/
#define PREVIEW_START   TI_REG_START+0x780
#define PREVIEW_END     TI_REG_START+0x7EC

/********************** PAL NTSC Encoder *******************************/
#define PAL_NTSC_ENC_START   TI_REG_START+0x800
#define PAL_NTSC_ENC_END     TI_REG_START+0x83A

/********************** CLOCK    ****************************************/
#define CLOCK_START   TI_REG_START+0x880
#define CLOCK_END     TI_REG_START+0x890


/********************** ECR      ****************************************/
#define ECR_START   TI_REG_START+0x900
#define ECR_END     TI_REG_START+0x906

/********************** TI VERSION **************************************/
#define TI_VER_START   TI_REG_START+0x906
#define TI_VER_END     TI_REG_START+0x908

#define TI_VER 0x32

/********************** HW_30A24 ****************************************/

/* this HW is used on the av for the usb/DC/HD status */
#define HAS_HW_30A24


/********************** DMA      ****************************************/
#define DMA_START   TI_REG_START+0xa38
#define DMA_END     TI_REG_START+0xa48

/********************** IDE BASE ****************************************/
#define IDE_BASE    0x02400000
#define IDE_END     0x02400400

/********************** CPLD     ****************************************/
#define CPLD_START       0x02000000
#define CPLD_END         0x03000000
#define CPLD_PORT_OFFSET 0x00500000

/********************** LCD      ****************************************/
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 176
