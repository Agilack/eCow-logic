/**
 * eCow-logic - reloc unit-test - bootloader level
 *
 * Copyright (c) 2015 Saint-Genest Gwenael <gwen@agilack.fr>
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef HARDWARE_H
#define HARDWARE_H
#include "types.h"

/* APB1 (0x4000xxxx) */
#define UART2_BASE  0x40006000
/* APB2 (0x4100xxxx) */
#define CRG_BASE    0x41001000
#define AFC_BASE    0x41002000
#define PADCON_BASE 0x41003000
/* AHB (0x42000000) */
#define MM_GPIOA    0x42000000
#define MM_GPIOB    0x43000000
#define MM_GPIOC    0x44000000

#define CRG_PLL_FCR  (CRG_BASE + 0x14)
#define CRG_PLL_IFSR (CRG_BASE + 0x20)

#define AFC_A    (AFC_BASE    + 0x00)
#define AFC_B    (AFC_BASE    + 0x40)
#define AFC_C    (AFC_BASE    + 0x80)
#define PADCON_C (PADCON_BASE + 0x80)

#define GPIOB_OUTENSET (MM_GPIOB + 0x10)
#define GPIOB_OUTENCLR (MM_GPIOB + 0x14)

#define MM_WR(addr, value)    (*(volatile u32 *)addr = (u32)value)

void hw_init(void);
u32  hw_getfreq(void);

inline u32  reg_rd(u32 reg);
inline void reg_wr(u32 reg, u32 value);
#endif
