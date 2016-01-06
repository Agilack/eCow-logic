/**
 * eCow-logic - Bootloader
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
#include "font.h"

void delay(u32 milliseconds);

void oled_cmd(u8 *cmd, int len);

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
    /* Set RST to high state for normal operations */
    reg_wr((MM_GPIOA + 0x800 + 0x100), (1 << 14));
    delay(2);
  }
  
  oled_dc(MODE_CMD);

  oled_cmd((u8 *)"\xAE", 1);     /* Set Display Off */
  oled_cmd((u8 *)"\xD5\x80", 2); /* Set Clock */
  oled_cmd((u8 *)"\xA8\x1F", 2); /* Set Multiplex Ratio */
  oled_cmd((u8 *)"\xD3\x00", 2); /* Set Display Offset */
  oled_cmd((u8 *)"\x40", 1);     /* Set Display Start Line */
  oled_cmd((u8 *)"\x8D\x14", 2); /* Configure Charge Pump  */
  oled_cmd((u8 *)"\xA1", 1);     /* Set Segment Remap */
  oled_cmd((u8 *)"\xC0", 1);     /* Set COM output scan direction */ /* GSG */
  oled_cmd((u8 *)"\xDA\00", 2);  /* COM pins hardware configuration */ /* 02 */
  oled_cmd((u8 *)"\x81\xCF", 2); /* Set Contrast Control */
  oled_cmd((u8 *)"\xD9\xF1", 2); /* Set precharge period */
  oled_cmd((u8 *)"\xDB\x40", 2); /* Set VCOMH deselect level */
  oled_cmd((u8 *)"\xA4", 1);     /* Set Entire Display On/Off */
  
  /* Configure display for clear */
  oled_cmd((u8 *)"\x20\x00", 2); /* Set Adressing Mode : Horizontal */
  oled_cmd((u8 *)"\xB0", 1);     /* Set current page = 0 */
  oled_cmd((u8 *)"\x21\x00\x7F", 3); /* Set lower column start address */
  
  /* Clear display memory */
  oled_dc(MODE_DATA);
  oled_cs(1);
  for (i = 0; i < 512; i++)
    spi_wr(0x00);
  oled_cs(0);
  
  /* Set Display On */
  oled_dc(MODE_CMD);
  oled_cmd((u8 *)"\xAF", 1);
}

void oled_putc(unsigned char c)  
{
  int index;
  int i;

  if (c & 0x80)
    return;

  index = c - ' ';
  oled_cs(1);
  for (i = 0; i < 8; i++)
    spi_wr( font[index][i] );
  oled_cs(0);
}

void oled_puts(char *s)
{
  while(*s)
    oled_putc(*s++);
}

void oled_line(int n)
{
  u8 lcmd;
  oled_dc(MODE_CMD);
  /* Set Adressing Mode : Page Adressing */
  oled_cmd((u8 *)"\x20\x02", 2);
  /* Set current page */
  lcmd = 0xB0 + n;
  oled_cmd(&lcmd, 1);
  /* Set lower column start address */
  oled_cmd((u8 *)"\x21\x00\x7F", 3);
  /* Switch back to data */
  oled_dc(MODE_DATA);
}

void oled_cmd(u8 *cmd, int len)
{
  oled_cs(1);
  while(len)
  {
    spi_wr(*cmd);
    cmd++;
    len--;
  }
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
    spi_flush();
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
