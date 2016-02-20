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
#include "W7500x.h"
#include "flash.h"
#include "iap.h"
#include "libc.h"
#include "miim.h"
#include "oled.h"
#include "spi.h"
#include "uart.h"
#include "W7500x_wztoe.h"
#include "dhcp.h"
#include "net_tftp.h"
#include "update.h"

static void boot_normal(void);
void main_loader(void);

int b2ds(char *d, int n);

const u8   mac_default[6] = {0x00, 0x08, 0xDC, 0x71, 0x72, 0x77};

int main(void)
{
  u8 *mac_addr;
  u32 fid;
  
  hw_init();
  uart_init();
  
  uart_puts("--=={ eCow-Logic Bootloader }==--\r\n");

  spi0_init();
  oled_init();
  flash_init(&fid);

  /* Init network layer */

  /* Set WZ_100US Register */
  setTIC100US( hw_getfreq() / 10000 );
  /* Get the MAC address from config */
  mac_addr = (u8 *)0x0003FF00;
  /* Is a MAC address is available, use it */
  if (mac_addr[0] != 0xFF)
  {
    setSHAR(mac_addr);
  }
  /* Else, set the default MAC address (debug only) */
  else
  {
    setSHAR(mac_default);
  }

  
  if (reg_rd(MM_GPIOA) & 2)
    boot_normal();
  else
    main_loader();
  return(-1);
}

void Jumper(u32 addr);

static void boot_normal(void)
{
  u32 ad;
  int i;
  
  /* Get the address of the Reset vector */
  ad = *(u32 *)0x00008004;
  if ( (ad == 0xFFFFFFFF) || (ad == 0x00000000) )
  {
    uart_puts("\r\n * Bad entry address. STOP\r\n");
    oled_pos(0, 0);
    oled_puts("Erreur firmware");
    while(1);
  }
  oled_pos(3, 0);
  oled_puts("eCow-Logic");
  
  uart_puts(" * Ready to start second stage at ");
  uart_puthex(ad);
  
  /* Wait ~1sec to allow auto-boot break */
  for (i = 0; i < 10; i++)
  {
    msleep(100);
    uart_putc('.');
  }
  uart_crlf();
  
  /* Stop Systick */
  *(u32 *)0xE000E010 = 0;
  
  /* Call main firmware ...*/
  Jumper(ad);
  
  while(1);
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

/* EOF */
