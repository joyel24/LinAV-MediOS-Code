/*
 * irq.c:
 *         av3xx IRQ init, mask, unmask, ack routines.
 * copyright:
 *         (C) 2001 RidgeRun, Inc. (http://www.ridgerun.com)
 * author: Gordon McNutt <gmcnutt@ridgerun.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 * WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 * USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/types.h>
#include <asm/arch/irq.h>


/* Acknowlede the IRQ. */

static inline void irq_ack(unsigned int irq)
{
	outw((1<<AV3XX_INTC_IRQ_SHIFT(irq)), AV3XX_INTC_IRQ_STATUS(irq));
}

/* Acknowledge the FIQ. */

static inline void fiq_ack(unsigned int irq)
{
	outw((1<<AV3XX_INTC_FIQ_SHIFT(irq)), AV3XX_INTC_FIQ_STATUS(irq));
}

/* Mask the IRQ. */

void av3xx_mask_irq(unsigned int irq)
{
	u_int32_t eint;
	u_int16_t mask;

	eint = AV3XX_INTC_IRQ_ENABLE(irq);
	mask = inw(eint);
	mask &= ~(1<<AV3XX_INTC_IRQ_SHIFT(irq));
	outw(mask, eint);
}

/* Unmask the IRQ. */

void av3xx_unmask_irq(unsigned int irq)
{
	u_int32_t eint;
	u_int16_t mask;

	eint = AV3XX_INTC_IRQ_ENABLE(irq);
	mask = inw(eint);
	mask |= (1<<AV3XX_INTC_IRQ_SHIFT(irq));
	outw(mask, eint);
}

/* Mask the IRQ and acknowledge it. */

void av3xx_mask_ack_irq(unsigned int irq)
{
	u_int32_t eint;
	u_int16_t mask;

	eint = AV3XX_INTC_IRQ_ENABLE(irq);
	mask = inw(eint);
	mask &= ~(1<<AV3XX_INTC_IRQ_SHIFT(irq));
	outw(mask, eint);

	outw((1<<AV3XX_INTC_IRQ_SHIFT(irq)), AV3XX_INTC_IRQ_STATUS(irq));
}


void av3xx_init_irq(void)
{


	/* disable all irqs */
	outw(0x0000, AV3XX_INTC_IRQ0_ENABLE);
	outw(0x0800, AV3XX_INTC_IRQ1_ENABLE);

	/* Set all to IRQ mode, not FIQ except WDT*/
	outw(0x0000, AV3XX_INTC_FISEL0);
	outw(0x0800, AV3XX_INTC_FISEL1);

	/* clear all status */
	outw(0xffff, AV3XX_INTC_FIQ0_STATUS);
	outw(0xffff, AV3XX_INTC_FIQ1_STATUS);
	outw(0xffff, AV3XX_INTC_IRQ0_STATUS);
	outw(0xffff, AV3XX_INTC_IRQ1_STATUS);

// setup irq on GIO
	outb(0xff,AV3XX_GIO_ENABLE_IRQ);
// setup corresponding GIO to input
	outw(inw(AV3XX_GIO_DIRECTION0)|0xFF,AV3XX_GIO_DIRECTION0);

/*
	printk("GIO IRQ setting: %x, direction: %x\n",inw(AV3XX_GIO_ENABLE_IRQ),inw(AV3XX_GIO_DIRECTION0));
	printk("FISEL 0:%x 1:%x\n",inw(AV3XX_INTC_FISEL0),inw(AV3XX_INTC_FISEL1));
	printk("INVERT 0:%x 1:%x\n",inw(AV3XX_GIO_INVERT0),inw(AV3XX_GIO_INVERT1));
*/




}
