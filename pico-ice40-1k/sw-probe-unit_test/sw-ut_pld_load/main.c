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
#include "uart.h"
#include "spi.h"
#include "pld.h"
#include "W7500x.h"

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

int main(void)
{
  hw_init();
  uart_init();
  spi_init();
  
  /* SysTick_Config */
  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("PLD loader Unit Test\r\n");
  
  pld_init();

  uart_puts(" * Try to load PLD ... ");
  pld_load((u8 *)0x20000000, 32768);
  uart_puts("done.\r\n");

  while(1)
    ;
}

void delay(__IO uint32_t milliseconds)
{
  TimingDelay = milliseconds;

  while(TimingDelay != 0);
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
/* EOF */
