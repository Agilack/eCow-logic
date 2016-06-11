/**
 * eCow-logic - Flash unit-test
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
#include "flash.h"
#include "W7500x.h"

#define USE_WRITE

void delay(__IO uint32_t milliseconds);
void dump(u8 *p, int len);

static __IO uint32_t TimingDelay;

extern u8 flash_vendor;
extern u8 flash_type;
extern u8 flash_capacity;

int main(void)
{
  u8  buffer[256];
  
  hw_init();
  uart_init();
  spi_init();
  
  /* SysTick_Config */
  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("SPI Flash Unit Test\r\n");

  flash_init();

  uart_puts("VENDOR=");      uart_puthex8(flash_vendor);   uart_putc(' ');
  uart_puts("DEVICE_TYPE="); uart_puthex8(flash_type);     uart_putc(' ');
  uart_puts("DEVICE_CAP=");  uart_puthex8(flash_capacity); uart_puts("\r\n");
  
  flash_read(0x000000, buffer, 256);
  
  dump(buffer, 64);

#ifdef USE_WRITE
  if (buffer[0] == 0xFF)
  {
    uart_puts(" * Sector empty ... try to write it ...\r\n");
    buffer[0] = 0xDE; buffer[1] = 0xAD; buffer[2] = 0xBE; buffer[3] = 0xEF;
    flash_write(0x000000, buffer, 16);
    
    flash_read(0x000000, buffer, 256);
    dump(buffer, 64);
  }
  else
    flash_erase(0x000000);
#endif

  uart_puts(" * Test complete.\r\n");

  while(1)
    ;
}

void dump(u8 *p, int len)
{
  int i;
  
  while(len > 0)
  {
    for (i = 0; i < 16; i++)
    {
      uart_puthex8(*p);
      p++;
      uart_putc(' ');
    }
    uart_puts("\r\n");
    len -= 16;
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
