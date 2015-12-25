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
#include "spi.h"

void spi_init(void)
{
  /* Configure SPI1 format */
  reg_wr(SPI1_CR0, 0x07);
  
  /* Configure main prescaler */
  reg_wr(SPI1_CLK, 10);
  
  /* Enable SPI1 */
  reg_wr(SPI1_CR1, 0x02);
  
  return;
}

void spi_wr(u32 c)
{
  while ( (reg_rd(SPI1_STATUS) & 0x02) == 0)
    ;
  reg_wr(SPI1_DATA, c);
}

void spi_wait(void)
{
  while( reg_rd(SPI1_STATUS) & 0x10 )
    ;
}
/* EOF */
