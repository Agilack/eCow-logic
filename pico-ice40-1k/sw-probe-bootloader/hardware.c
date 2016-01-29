/**
 * eCow-logic - Bootloader
 *
 * Copyright (c) 2016 Saint-Genest Gwenael <gwen@agilack.fr>
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

void hw_init(void)
{
  hw_setup_clock(0);

  /* Configure SPI0 (for Flash and OLED) */
  reg_wr(AFC_A + 0x18, 0); /* PA_06 : Alternate Function 0 (SPI CLK)      */
  reg_wr(AFC_A + 0x1C, 0); /* PA_07 : Alternate Function 0 (SPI MISO)     */
  reg_wr(AFC_A + 0x20, 0); /* PA_08 : Alternate Function 0 (SPI MOSI)     */
  
  /* Configure SPI1 (for PLD) */
  reg_wr(AFC_B + 0x04, 0); /* PB_01 : Alternate Function 0 (SPI CLK)      */
  reg_wr(AFC_B + 0x08, 0); /* PB_02 : Alternate Function 0 (SPI MISO)     */
  reg_wr(AFC_B + 0x0C, 0); /* PB_03 : Alternate Function 0 (SPI MOSI)     */

  /* Configure PLD signals */
  reg_wr(MM_GPIOB + 0x10, 0x01); /* Set GPIOB-0 as output (PLD CS)           */
  reg_wr(MM_GPIOC + 0x10, 0x20); /* Set GPIOC-5 as output (PLD reset)        */
  reg_wr(AFC_B + 0x00, 1); /* PB_00 : Alternate Function 1 (GPIO for CS)     */
  reg_wr(AFC_C + 0x10, 1); /* PC_04 : Alternate Function 1 (GPIO for "done") */
  reg_wr(AFC_C + 0x14, 1); /* PC_05 : Alternate Function 1 (GPIO for rst)    */
  reg_wr(PADCON_C + 0x10, 0x62); /* PC_04: Enable pull-up ("done")           */
  
  /* Configure external SPI Flash */
  reg_set(MM_GPIOA + 0x10, 0x0020); /* Set GPIOA 5 as outputs             */
  reg_wr(AFC_A + 0x14, 1); /* PA_05 : Alternate Function 1 (GPIO for CS)  */
  
  /* Configure MIIM : MDIO */
  reg_wr(MM_GPIOB + 0x10, (1 << 14)); /* Set GPIO-14 (MDIO) output enable bit */
  reg_wr(AFC_B + 0x38, 1);            /* Set Alternate Function 1 (now works as GPIO) */
  /* Configure MIIM : MDC */
  reg_wr(MM_GPIOB + 0x10, (1 << 15)); /* Set GPIO-14 (MDC) output enable bit */
  reg_wr(AFC_B + 0x3C, 1);            /* Set Alternate Function 1 (now works as GPIO) */

  /* Configure OLED */
  reg_set(MM_GPIOA + 0x10, 0x9804); /* Set GPIOA 2,11,12,15 as outputs       */
  reg_wr(AFC_A + 0x08, 1);    /* PA_02 : Alternate Function 1 (GPIO for SW)  */
  reg_wr(AFC_A + 0x2C, 1);    /* PA_11 : Alternate Function 1 (GPIO for D/C) */
  reg_wr(AFC_A + 0x30, 1);    /* PA_12 : Alternate Function 1 (GPIO for CS)  */
  reg_wr(AFC_A + 0x3C, 1);    /* PA_15 : Alternate Function 1 (GPIO for EN)  */
  
  /* Configure PA10 as LCD reset */
  reg_set(MM_GPIOA + 0x10, 0x0400); /* Set GPIOA-10 as output */
  reg_wr(AFC_A + 0x28, 1);          /* Set alternate function as GPIO */
  /* Set RST low to force display reset */
  reg_wr((MM_GPIOA + 0x800 + 0x10), 0);
}

void hw_setup_clock(int speed)
{
  if (speed)
  {
    /* Set external oscillator as PLL clock source */
    reg_wr(CRG_PLL_IFSR, 0x01);
  
    /* Update PLL for 20MHz MCLK */
    reg_wr(CRG_PLL_FCR, 0x00050200);
  }
  else
  {
    reg_wr(CRG_OSC_TRIM, reg_rd(INFO_OSC));
    /* Set internal oscillator as PLL clock source */
    reg_wr(CRG_PLL_IFSR, 0x00);
    /* Update PLL for 8MHz MCLK */
    reg_wr(CRG_PLL_FCR, 0x00050500);
  }
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

inline void reg_set(u32 reg, u32 value)
{
  *(volatile u32 *)reg = (*(volatile u32 *)reg | value);
}
/* EOF */
