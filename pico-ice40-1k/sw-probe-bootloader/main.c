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
#include "uart.h"
#include "W7500x.h"
#include "spi.h"
#include "oled.h"
#include "miim.h"
#include "W7500x_wztoe.h"
#include "dhcp.h"

static void boot_normal(void);
static void boot_loader(void);

int b2ds(char *d, int n);

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

int main(void)
{
  hw_init();
  uart_init();
  
  /* SysTick_Config */
  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("Bootloader Unit Test\r\n");
  
  if (reg_rd(MM_GPIOA) & 2)
    boot_normal();
  else
    boot_loader();
  return(-1);
}

static void boot_normal(void)
{
  uart_puts(" * Boot in 'normal' mode not implemented yet !\r\n");
  while(1);
}

static void boot_loader(void)
{
  uint8_t mac_addr[6] = {0x00, 0x08, 0xDC, 0x71, 0x72, 0x77};
  char buffer[2048];
  char msg[16];
  char tmp[4];
  u8 dhcp_state;
  int step;
  
  uart_puts(" * Start LOADER mode \r\n");
  
  spi0_init();
  
  oled_init();
  oled_line(4);
  oled_puts("eCowL     Loader");

  /* Set WZ_100US Register */
  setTIC100US( hw_getfreq() / 10000 );

  /* PHY Link Check via gpio mdio */
  while( link() == 0x0 )
  {  
    uart_putc('.');
    delay(500);
  }
  uart_puts(" * Link ok\r\n");
  oled_line(1);
  oled_puts("Reseau ok");
  
  setSHAR(mac_addr);  
  DHCP_init(2, (u8 *)buffer);
  
  step = 0;
  while(1)
  {
    dhcp_state = DHCP_run();
    if (dhcp_state != DHCP_IP_LEASED)
      continue;
    if (step == 0)
    {
      char * pnt;
      getSIPR(tmp);
      pnt = msg;
      pnt += b2ds(msg, tmp[0]); *pnt++ = '.';
      pnt += b2ds(pnt, tmp[1]); *pnt++ = '.';
      pnt += b2ds(pnt, tmp[2]); *pnt++ = '.';
      pnt += b2ds(pnt, tmp[3]); *pnt = 0;
      
      oled_line(1);
      oled_puts(msg);
      uart_puts("LEASED\r\n");
      step ++;
    }
  }
}

int b2ds(char *d, int n)
{
  int count = 0;
  
  if (n > 99)
  {
    *d = (n / 100) + '0';
    n -= ((n / 100) * 100);
    d++;
    count++;
  }
  if ( (n > 9) || count)
  {
    *d = (n / 10) + '0';
    n -= ((n / 10) * 10);
    d++;
    count ++;
  }
  *d = n + '0';
  count ++;
  
  return(count);
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
