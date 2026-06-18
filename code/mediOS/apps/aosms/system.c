/*
    Copyright (C) 1998, 1999, 2000  Charles Mac Donald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "shared.h"


__IRAM_DATA t_bitmap bitmap;
t_cart cart;
__IRAM_DATA t_input input;

struct
{
    char reg[64];
}ym2413;

void system_init(int rate)
{
    /* Initialize the VDP emulation */
    vdp_init();

    /* Initialize the SMS emulation */
    sms_init();

    /* Initialize the look-up tables and related data */
    render_init();

    /* Don't save SRAM by default */
    sms.save = 0;

    /* Clear emulated button state */
    memset(&input, 0, sizeof(t_input));
}

void system_shutdown(void)
{
}


void system_reset(void)
{
    cpu_reset();
    vdp_reset();
    sms_reset();
    render_reset();
}


void system_save_state(int fd)
{
    /* Save VDP context */
    write(fd,&vdp, sizeof(t_vdp));
    write(fd,vdp_vram,sizeof(vdp_vram));

    /* Save SMS context */
    write(fd,&sms, sizeof(t_sms));
    write(fd,sms_ram,sizeof(sms_ram));
    write(fd,sms_dummy,sizeof(sms_dummy));
    write(fd,sms_sram,sizeof(sms_sram));

    /* Save Z80 context */
#ifdef ASM_CPU
    write(fd,&Z80, sizeof(Z80_Regs));
#else
    write(fd,c_Z80_Context, sizeof(c_Z80_Regs));
    write(fd,&after_EI, sizeof(int));
#endif

    /* Save YM2413 registers */
    write(fd,&ym2413.reg[0], 0x40);

    /* Save SN76489 context */
//    write(fd,&sn[0], sizeof(t_SN76496));
}


void system_load_state(int fd)
{
    int i;
    uint8 reg[0x40];

    /* Initialize everything */
    cpu_reset();
    system_reset();

    /* Load VDP context */
    read(fd,&vdp, sizeof(t_vdp));
    read(fd,vdp_vram,sizeof(vdp_vram));

    /* Load SMS context */
    read(fd,&sms, sizeof(t_sms));
    read(fd,sms_ram,sizeof(sms_ram));
    read(fd,sms_dummy,sizeof(sms_dummy));
    read(fd,sms_sram,sizeof(sms_sram));

    /* Load Z80 context */
#ifdef ASM_CPU
    read(fd,&Z80, sizeof(Z80_Regs));
    Z80_initFctPtrsAndRebase(); //DRZ80 saves a lot of binary code dependent stuff, regenerate it
#else
    read(fd,c_Z80_Context, sizeof(c_Z80_Regs));
    read(fd,&after_EI, sizeof(int));
#endif

    /* Load YM2413 registers */
    read(fd,reg, 0x40);

    /* Load SN76489 context */
//    read(fd,&sn[0], sizeof(t_SN76496));

    /* Restore callbacks */
#ifdef ASM_CPU
#else
    z80_set_irq_callback(sms_irq_callback);
#endif

    cpu_readmap[0] = cart.rom + 0x0000; /* 0000-3FFF */
    cpu_readmap[1] = cart.rom + 0x2000;
    cpu_readmap[2] = cart.rom + 0x4000; /* 4000-7FFF */
    cpu_readmap[3] = cart.rom + 0x6000;
    cpu_readmap[4] = cart.rom + 0x0000; /* 0000-3FFF */
    cpu_readmap[5] = cart.rom + 0x2000;
    cpu_readmap[6] = sms_ram;
    cpu_readmap[7] = sms_ram;

    cpu_writemap[0] = sms_dummy;
    cpu_writemap[1] = sms_dummy;
    cpu_writemap[2] = sms_dummy;
    cpu_writemap[3] = sms_dummy;
    cpu_writemap[4] = sms_dummy;
    cpu_writemap[5] = sms_dummy;
    cpu_writemap[6] = sms_ram;
    cpu_writemap[7] = sms_ram;

    sms_mapper_w(3, sms.fcr[3]);
    sms_mapper_w(2, sms.fcr[2]);
    sms_mapper_w(1, sms.fcr[1]);
    sms_mapper_w(0, sms.fcr[0]);

    /* Force full pattern cache update */
    is_vram_dirty = 1;
    memset(vram_dirty, 1, 0x200);

    /* Restore palette */
    for(i = 0; i < PALETTE_SIZE; i += 1)
        palette_sync(i);

}




