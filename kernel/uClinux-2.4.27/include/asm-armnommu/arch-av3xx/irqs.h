/***********************************************************************
 * include/asm-armnommu/arch-dm270/irqs.h
 *
 *   Derived from asm/arch-armnommu/arch-c5471/irqs.h
 *
 *   Copyright (C) 2004 InnoMedia Pte Ltd. All rights reserved.
 *   cheetim_loh@innomedia.com.sg  <www.innomedia.com>
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
 ***********************************************************************/


#ifndef __ASM_ARCH_IRQS_H__
#define __ASM_ARCH_IRQS_H__

#include <linux/config.h>

#define AV3XX_IRQ_TMR0           0
#define AV3XX_IRQ_TMR1           1
#define AV3XX_IRQ_TMR2           2
#define AV3XX_IRQ_TMR3           3

#define AV3XX_IRQ_OSD            7 // Seems to just interrupt really fast
                                	// Does this mean end of osd update?

#define AV3XX_IRQ_UART0         12 // When a character is recieved...
#define AV3XX_IRQ_UART1         13

#define AV3XX_IRQ_EXT0          18 // GIO0 ON button when enabled in gio
#define AV3XX_IRQ_EXT1          19
#define AV3XX_IRQ_EXT2          20
#define AV3XX_IRQ_EXT3          21
#define AV3XX_IRQ_EXT4          22 // GIO4 MAS_DATA_REQUEST
#define AV3XX_IRQ_IR_VDR        24 // GIO6 IR data
#define AV3XX_IRQ_EXT6			25
#define AV3XX_IRQ_EXT7			26
#define AV3XX_IRQ_EXT8			27 // -
#define AV3XX_IRQ_EXT9			28 // |
#define AV3XX_IRQ_EXT10			29 // |	are they really present ?
#define AV3XX_IRQ_EXT11			30 // |
#define AV3XX_IRQ_EXT12			31 // -
#define IRQ_TIMER                     (AV3XX_IRQ_TMR0)
#define NR_IRQS                       (AV3XX_IRQ_EXT12+1)

/* flags for request_irq().  IRQ_FLG_STD is apparently a uClinux-ism,
 * so I'll keep it around even though it is just a mapping for the
 * real SA_INTERRUPT. -- skj
 */

#define IRQ_FLG_STD                   (SA_INTERRUPT)

#endif /* __ASM_ARCH_IRQS_H__ */
