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
#include "spi.h"

void spi0_init(void)
{
  /* Configure SPI0 format */
  reg_wr(SPI0_CR0, 0x07);
  /* Configure main prescaler */
  reg_wr(SPI0_CLK, 2);
  /* Enable SPI0 */
  reg_wr(SPI0_CR1, 0x02);
  
  return;
}

u8   spi_rd(void)
{
  while( (reg_rd(SPI0_STATUS) & 0x04) == 0)
    ;
  return( reg_rd(SPI0_DATA) );
}

void spi_wr(u32 c)
{
  while ( (reg_rd(SPI0_STATUS) & 0x02) == 0)
    ;
  reg_wr(SPI0_DATA, c);
}

void spi_flush(void)
{
  while (reg_rd(SPI0_STATUS) & 0x04)
    reg_rd(SPI0_DATA);
}

void spi_wait(void)
{
  while( reg_rd(SPI0_STATUS) & 0x10 )
    ;
}
/* EOF */
