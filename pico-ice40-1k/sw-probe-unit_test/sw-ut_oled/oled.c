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
#include "oled.h"
#include "spi.h"

void delay(u32 milliseconds);

void oled_init(void)
{
  int i;
  
  /* Activate data multiplexer (LCD_EN = 0) */
  reg_wr((MM_GPIOA + 0x800 + 0x200), 0);
  /* Set multiplexer for W7500 control (LCD_SW = 0) */
  reg_wr((MM_GPIOA + 0x400 + 0x010), 0);
  /* Set default value for CS */
  oled_cs(0);
  
  /* GSG: add reset pulse for tests */
  {
    /* Set RST to low state for reset */
    reg_wr((MM_GPIOA + 0x800 + 0x100), 0);
    delay(100);
    /* Set RST to higt state for normal operations */
    reg_wr((MM_GPIOA + 0x800 + 0x100), (1 << 14));
    delay(2);
  }
  
  oled_dc(MODE_CMD);

  /* Set Display Off */
  oled_cs(1);
  spi_wr(0xAE);
  oled_cs(0);
  /* Set Clock */
  oled_cs(1);
  spi_wr(0xD5);
  spi_wr(0x80);
  oled_cs(0);
  /* Set Multiplex Ratio */
  oled_cs(1);
  spi_wr(0xA8);
  spi_wr(0x1F);
  oled_cs(0);
  /* Set Display Offset */
  oled_cs(1);
  spi_wr(0xD3);
  spi_wr(0x00);
  oled_cs(0);
  /* Set Display Start Line */
  oled_cs(1);
  spi_wr(0x40);
  oled_cs(0);
  /* Configure Charge Pump  */
  oled_cs(1);
  spi_wr(0x8D);
  spi_wr(0x14);
  oled_cs(0);
  /* Set Segment Remap */
  oled_cs(1);
  spi_wr(0xA1);
  oled_cs(0);
  /* COM pins hardware configuration */
  oled_cs(1);
  spi_wr(0xDA);
  spi_wr(0x00);
  oled_cs(0);
  /* Set Contrast Control */
  oled_cs(1);
  spi_wr(0x81);
  spi_wr(0xCF);
  oled_cs(0);
  /* Set precharge period */
  oled_cs(1);
  spi_wr(0xD9);
  spi_wr(0x1F);
  oled_cs(0);
  /* Set VCOMH deselect level */
  oled_cs(1);
  spi_wr(0xDB);
  spi_wr(0x40);
  oled_cs(0);
  /* Set Entire Display On/Off */
  oled_cs(1);
  spi_wr(0xA4);
  oled_cs(0);
  /* Set Adressing Mode : Page Adressing */
  oled_cs(1);
  spi_wr(0x20);
  spi_wr(0x02);
  oled_cs(0);
  /* Set current page = 0 */
  oled_cs(1);
  spi_wr(0xB0);
  oled_cs(0);
  /* Set lower column start address */
  oled_cs(1);
  spi_wr(0x21);
  spi_wr(0x00);
  spi_wr(0x7F);
  oled_cs(0);
  
  /* Delay ... */
  for (i = 0; i < 0x1FF; i++)
    ;
  for (i = 0; i < 0x1FF; i++)
    ;
  /* Set Display On */
  oled_cs(1);
  spi_wr(0xAF);
  oled_cs(0);
}

void oled_wr(u8 c)
{
  oled_cs(1);
  spi_wr(c);
  oled_cs(0);
}

void oled_cs(int en)
{
  if (en)
    reg_wr((MM_GPIOA + 0x800 + 0x040) , 0);
  else
  {
    spi_wait();
    reg_wr((MM_GPIOA + 0x800 + 0x040) , (1 << 12));
  }
}

void oled_dc(int dc)
{
  if (dc)
    reg_wr((MM_GPIOA + 0x800 + 0x020) , (1 << 11));
  else
    reg_wr((MM_GPIOA + 0x800 + 0x020) , 0);
}
/* EOF */
