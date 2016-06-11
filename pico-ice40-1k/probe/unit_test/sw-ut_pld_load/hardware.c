/**
 * eCow-logic - PLD loader unit-test
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

  /* Configure SPI1 */
  reg_wr(AFC_B + 0x04, 0); /* PB_01 : Alternate Function 0 (SPI CLK)      */
  reg_wr(AFC_B + 0x08, 0); /* PB_02 : Alternate Function 0 (SPI MISO)     */
  reg_wr(AFC_B + 0x0C, 0); /* PB_03 : Alternate Function 0 (SPI MOSI)     */
  
  /* Configure PLD signals */
  reg_wr(MM_GPIOB + 0x10, 0x01); /* Set GPIOB-0 as output (PLD CS)        */
  reg_wr(MM_GPIOC + 0x10, 0x20); /* Set GPIOC-5 as output (PLD reset)     */
  reg_wr(AFC_B + 0x00, 1); /* PB_00 : Alternate Function 1 (GPIO for CS)  */
  reg_wr(AFC_C + 0x14, 1); /* PC_05 : Alternate Function 1 (GPIO for rst) */
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
