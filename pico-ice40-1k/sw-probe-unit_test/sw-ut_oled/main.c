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
#include "uart.h"
#include "spi.h"
#include "oled.h"
#include "W7500x.h"

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

int main(void)
{
  int toggle;
  int i;
  
  hw_init();
  uart_init();
  spi_init();
  
  /* SysTick_Config */
  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("OLED Unit Test\r\n");
  
  oled_init();
  oled_dc(MODE_DATA);

  i = 0xA5;
  toggle = 0;
  while(1)
  {
        if (toggle)
        {
          uart_putc('0');
          toggle = 0;
          delay(250);
        }
        else
        {
          uart_putc('1');
          oled_wr(i++);
          toggle = 1;
          delay(250);
        }
  }
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
