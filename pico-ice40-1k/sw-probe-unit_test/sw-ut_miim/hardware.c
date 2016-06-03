/**
 * eCow-logic - PHY management interface (MIIM) unit-test
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

  /* Configure MIIM : MDIO */
  reg_wr(MM_GPIOB + 0x10, (1 << 14)); /* Set GPIO-14 (MDIO) output enable bit */
  reg_wr(AFC_B + 0x38, 1);            /* Set Alternate Function 1 (now works as GPIO) */
  /* Configure MIIM : MDC */
  reg_wr(MM_GPIOB + 0x10, (1 << 15)); /* Set GPIO-14 (MDC) output enable bit */
  reg_wr(AFC_B + 0x3C, 1);            /* Set Alternate Function 1 (now works as GPIO) */
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
/* EOF */
