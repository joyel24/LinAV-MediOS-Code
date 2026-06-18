#ifndef __ncpuZ_h__
#define __ncpuZ_h__


extern void cpu_pending_irq(void);
extern void cpu_irq(void);
extern void cpu_nmi(void);
extern int cpu_getcycles(void);
extern int cpu_exec(uint32 total_cycles);
extern void cpu_burn(uint32 burn_cycles);
extern void cpu_release(void);
extern void init_sram(uint8 *sram);
extern void reset_cpu(void);
extern void init_cpu(uint8 *nesram);
extern void set_cpu_bank_full(uint8 *bank0,uint8 *bank1,uint8 *bank2,uint8 *bank3);
extern void set_cpu_bank0(uint8 *bank0);
extern void set_cpu_bank1(uint8 *bank1);
extern void set_cpu_bank2(uint8 *bank2);
extern void set_cpu_bank3(uint8 *bank3);
extern void reload_fast_pc(void);

extern void get_context(uint32* nesregs);
extern void set_context(uint32* nesregs);

#endif
