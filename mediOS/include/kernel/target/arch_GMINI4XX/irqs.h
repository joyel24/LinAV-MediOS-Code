/* 
*   include/kernel/target/arch_G4XX/irqs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __IRQS_ARCH_H
#define __IRQS_ARCH_H

#define NR_IRQS              32

#define IRQ_IDE              15  // ide

#define IRQ_BTN_ON           18  // GIO0

#define IRQ_MAS_DATA         22  // GIO4
#define IRQ_IR               24  // GIO6

#endif
