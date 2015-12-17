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
#include "W7500x.h"
#include "iap.h"
#include "libc.h"
#include "miim.h"
#include "oled.h"
#include "spi.h"
#include "uart.h"
#include "W7500x_wztoe.h"
#include "dhcp.h"
#include "net_tftp.h"

static void boot_normal(void);
static void boot_loader(void);

int b2ds(char *d, int n);

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

extern uint8_t DHCP_server_ip[4];

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
  tftp tftp_session;
  int  tftp_block;
  u32  iap_addr;
  
  uart_puts(" * Start LOADER mode \r\n");
  
  spi0_init();
  
  oled_init();
  oled_line(3);
  oled_puts("eCowL     Loader");

  /* Set WZ_100US Register */
  setTIC100US( hw_getfreq() / 10000 );

  /* PHY Link Check via gpio mdio */
  oled_line(1);
  oled_puts("Init reseau ...");
  while( link() == 0x0 )
  {  
    uart_putc('.');
    delay(500);
  }
  uart_puts(" * Link ok\r\n");
  oled_line(1);
  oled_puts("Reseau (DHCP)   ");
  
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
      
      uart_puts("DHCP: "); uart_puts(msg); uart_puts("\r\n");
      
      oled_line(1);
      oled_puts("                ");
      oled_line(1);
      oled_puts(msg);
      
      tftp_init(&tftp_session);
      tftp_session.server[0] = DHCP_server_ip[0];
      tftp_session.server[1] = DHCP_server_ip[1];
      tftp_session.server[2] = DHCP_server_ip[2];
      tftp_session.server[3] = DHCP_server_ip[3];
      iap_addr = 0x8000;
      
      step ++;
    }
    if (step == 1)
    {
      tftp_run(&tftp_session);
      if (tftp_session.state == 2)
      {
        char str[17];
        int len;
        
        if (tftp_session.lastblock != tftp_block)
        {
          tftp_block = tftp_session.lastblock;
          
          strcpy(str, "TFTP: load      ");
          b2ds(&str[11], tftp_block);
          oled_line(0);
          oled_puts(str);
          
          len = 0;
          if (tftp_session.length > 12)
          {
            if ( (iap_addr & 0x0FFF) == 0)
              iap_erase(iap_addr);
            len = tftp_session.length - 12;
            iap_write(iap_addr, &tftp_session.buffer[12], len);
            iap_addr += len;
          }
        }
      }
      if (tftp_session.state == 3)
      {
        oled_line(0);
        oled_puts("TFTP: complet.");
      }
      if (tftp_session.state == 99)
      {
        oled_line(0);
        oled_puts("TFTP: erreur 1  ");
        step = 2;
      }
    }
  }
}

int b2ds(char *d, int n)
{
  int count = 0;
  
  if (n > 999)
  {
    *d = (n / 1000) + '0';
    n -= ((n / 1000) * 1000);
    d++;
    count++;
  }
  if ((n > 99) || count)
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
