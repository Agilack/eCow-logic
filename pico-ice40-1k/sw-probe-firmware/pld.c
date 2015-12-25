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
#include "pld.h"
#include "spi.h"

static void pld_cs(int en);

void pld_init(void)
{
  /* Put PLD in reset state */
  reg_wr((MM_GPIOC + 0x400 + 0x80), 0);
  /* De-assert CS to avoid SPI transaction */
  pld_cs(0);
}

void pld_load_start(void)
{
  int i;
  
  /* Assert CS before RST release to request PLD to be spi slave */
  pld_cs(1);
  
  /* Wait a short time */
  for (i = 0; i < 2500; i++)
    __asm volatile ("nop");
  
  /* un-assert reset */
  reg_wr((MM_GPIOC + 0x400 + 0x80) , (1 << 5));
  
  /* Wait a short time */
  for (i = 0; i < 2500; i++)
    __asm volatile ("nop");
}

void pld_load(const unsigned char *bitstream, u32 len)
{
  int i;
  
  /* Send datas */
  for (i = 0; i < len; i++)
  {
    spi_wr(*bitstream);
    bitstream++;
  }
  
  return;
}

void pld_load_end(void)
{
  int i;
  
  /* Send more bytes to allow programming process to finish */
  for (i = 0; i < 50; i++)
    spi_wr(0x00);
  
  /* End :) */
  pld_cs(0);
}

static void pld_cs(int en)
{
  if (en)
    reg_wr((MM_GPIOB + 0x400 + 0x04) , 0);
  else
  {
//    spi_wait();
    reg_wr((MM_GPIOB + 0x400 + 0x04) , 1);
  }
}
/* EOF */
