/**
 * eCow-logic - IAP unit-test
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

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

#define IAP_ENTRY 0x1FFF1001

u8 buffer[512];

int main(void)
{
  u32 ad;
  u32 id;
  u32 dst_addr;
  u8 *src_addr;
  u32 size;
  int i;
  
  hw_init();
  uart_init();
  
  /* SysTick_Config */
//  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("IAP test\r\n");

  uart_puts(" * Information Block (lock info) :\r\n");
  uart_dump( (u8 *)0x0003FC00, 256);
  uart_puts("\r\n");
  uart_puts(" * Information Block (reserved) :\r\n");
  uart_dump( (u8 *)0x0003FD00, 256);
  uart_puts("\r\n");

  uart_puts("Data before \r\n");
  uart_dump( (u8 *)0x00008000, 256);
  uart_puts("\r\n\n");
  
  for (i = 0; i < 512; i++)
    buffer[i] = i;
  
  id = 19; // 0x22;
  dst_addr = 0x00008000; //0x0003FE00;
  src_addr = 0; //buffer;
  size = 0; //128;
  
  reg_wr((MM_GPIOA + 0x800 + 0x40), (1 << 12));
  ((void(*)(uint32_t,uint32_t,uint8_t*,uint32_t))IAP_ENTRY)(id, dst_addr, src_addr, size);
  reg_wr((MM_GPIOA + 0x800 + 0x40), 0);
  
//  uart_puthex( *(u16 *)0x1003fe00 );
//  uart_puts("DAT1 ");
//  uart_puthex( *(u32 *)0x1003ff00 );
//  uart_puts("\r\n");
//  ad = 0x1FFF1000;
//  uart_dump((u8 *)ad, 4096);
  
  /* never reach this point ... */
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
