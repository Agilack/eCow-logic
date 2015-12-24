/**
 * eCow-logic - Embedded probe main firmware
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
#include "hardware.h"
#include "types.h"

static void hw_setup_clock(void);

void hw_init(void)
{
  hw_setup_clock();
}

static void hw_setup_clock(void)
{
  /* Set external oscillator as PLL clock source */
  reg_wr(CRG_PLL_IFSR, 0x01);
  
  /* Update PLL for 20MHz MCLK */
  reg_wr(CRG_PLL_FCR, 0x00050200);
}

u32 hw_getfreq(void)
{
  u32 v;
  u32 od, n, m;
  
  /* Get the current PLL configuration */
  v = reg_rd(CRG_PLL_FCR);
  
  /* Extract PLL values */
  od = ( 1 << (v & 0x01)) * (1 << ( (v & 0x02) >> 1) );
  n  = (v >>  8) & 0x3F;
  m  = (v >> 16) & 0x3F;
  
  /* Compute current frequency */
  v = 0x007A1200; /* External oscillator = 8MHz */
  v = v * m / n * 1 / od;
  
  return v;
}

inline u32  reg_rd(u32 reg)
{
  return( *(volatile u32 *)reg );
}

inline void reg_wr(u32 reg, u32 value)
{
  *(volatile u32 *)reg = value;
}

void WIZCHIP_WRITE(u32 Addr, u8 Data)
{
    //WIZCHIP_CRITICAL_ENTER();
    *(volatile u8 *)(Addr) = Data;
    //WIZCHIP_CRITICAL_EXIT();
}
/* EOF */
