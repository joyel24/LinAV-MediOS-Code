/*
 * linux/include/asm-armnommu/arch-dm270/system.h
 */

static inline void arch_idle(void)
{
  while (!current->need_resched && !hlt_counter);
}

extern inline void arch_reset(char mode)
{
	/* Jump into ROM at 0x00100000 */
	/* added by oxygen77 */
	printk("let's go to reset\n");
	cpu_reset(FLASH_MEM_BASE);
}
