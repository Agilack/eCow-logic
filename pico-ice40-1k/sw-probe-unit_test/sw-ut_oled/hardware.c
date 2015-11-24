/**
 * eCow-logic - OLED unit-test
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

  /* Configure SPI0 */
  reg_wr(AFC_A + 0x14, 0); /* PA_05 : Alternate Function 0 (SPI CS)       */
  reg_wr(AFC_A + 0x18, 0); /* PA_06 : Alternate Function 0 (SPI CLK)      */
  reg_wr(AFC_A + 0x1C, 0); /* PA_07 : Alternate Function 0 (SPI MISO)     */
  reg_wr(AFC_A + 0x20, 0); /* PA_08 : Alternate Function 0 (SPI MOSI)     */
  
  /* Configure OLED */
  reg_wr(MM_GPIOA + 0x10, 0x9804); /* Set GPIOA 2,11,12,15 as outputs     */
  reg_wr(AFC_A + 0x08, 1); /* PA_02 : Alternate Function 1 (GPIO for SW)  */
  reg_wr(AFC_A + 0x2C, 1); /* PA_11 : Alternate Function 1 (GPIO for D/C) */
  reg_wr(AFC_A + 0x30, 1); /* PA_12 : Alternate Function 1 (GPIO for CS)  */
  reg_wr(AFC_A + 0x3C, 1); /* PA_15 : Alternate Function 1 (GPIO for EN)  */
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
/* EOF */
