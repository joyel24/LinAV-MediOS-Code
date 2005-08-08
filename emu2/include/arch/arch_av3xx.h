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

#define ARCH_NAME         "AV3XX"

/************************************************************ Memory Zone */

#define RESET_VECTOR      0x00000000

#define IRAM_START        0x00000000
#define IRAM_END          0x00008000
#define TI_REG_START      0x00030000
#define TI_REG_END        0x00040000

/*IRAM should be the same on all arch */



#define FLASH_START       0x100000
#define FLASH_END         0x180000

#define SDRAM_START       0x03000000
#define SDRAM_END         0x04000000


/************************************************************ initial state */

#define START_ADDR        SDRAM_START
#define STACK_INIT        (IRAM_END - 0x4) 
#define RESET_INIT_VAL    0xEA03FFFE     
#define INIT_MODE         M_SVC

//#define HAS_VID0

/* Special init for this arch: enable debug on UART */
#define ARCH_INIT   \
{                   \
    char * str="DeBuGuNlOcKeD_42";  \
    for(int i=0;*(str+i);i++)       \
        write((0x107FF0+i),(char)*(str+i),1); \
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

/********************** IRQ      ****************************************/
#define IRQ_START   TI_REG_START+0x500
#define IRQ_END     TI_REG_START+0x560

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

/********************** VIDEO    ****************************************/
#define VID_START   TI_REG_START+0x800
#define VID_END     TI_REG_START+0x880

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
#define CPLD_PORT_OFFSET 0x00600000

/********************** LCD      ****************************************/
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
