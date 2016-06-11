/**
 * eCow-logic - PHY management interface (MIIM) unit-test
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
#include "W7500x.h"
#include "miim.h"

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

int main(void)
{
  u32 v;
  int i;
  
  hw_init();
  uart_init();
  
  /* SysTick_Config */
  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("MIIM Unit Test\r\n");
  
  uart_puts(" * Detect PHY address :");
  for (v = 0; v < 0x20; v++)
  {
    if ((v % 4) == 0)
      uart_puts("\r\n");
    uart_puthex8(v); uart_putc('=');
    uart_puthex16( detect(v) );
    uart_putc(' ');
  }
  uart_puts("\r\n\n");
  
  uart_puts(" * Show all registers :");
  for (v = 0; v < 0x20; v++)
  {
    if ((v % 4) == 0)
      uart_puts("\r\n");
    uart_puthex8(v); uart_putc('=');
    uart_puthex16( mdio_read(v, 0x00) );
    uart_putc(' ');
  }
  uart_puts("\r\n\n");
  
  i = 0;
  while(1)
  {
    /* Show the counter value */
    uart_putc('['); uart_puthex(i); uart_puts("] ");
    /* Show the raw value */
    v = mdio_read(0x01, PHY_ADDR);
    uart_puts("status="); uart_puthex16(v);
    /* Show link up/down according to value */
    uart_puts(" => PHY ");
    if ( link() )
      uart_puts("OK :)\r");
    else
      uart_puts("KO :(\r");
    i++;
    delay(1000);
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
