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
#include "iap.h"
#include "libc.h"
#include "miim.h"
#include "oled.h"
#include "uart.h"
//#include "dhcp.h"
#include "net_tftp.h"
#include "update.h"

const char upd_file[9] = "ecow.upd";

void ldr_tftp(dhcp_session *dhcp);

void main_loader(void)
{
  char buffer[2048];
  char msg[16];
  char tmp[4];
  char dhcp_file[128];
  u8 dhcp_state;
  dhcp_session dhcp_session;
  int  step;
  char *pnt;
  
  uart_puts(" * Start LOADER mode \r\n");
  
  oled_pos(3, 0);
  oled_puts("eCowL     Loader");

  /* PHY Link Check via gpio mdio */
  oled_pos(1, 0);
  oled_puts("Init reseau ...");
  step = 0;
  while( link() == 0x0 )
  {
    uart_putc('.');
    oled_pos(1, 13);
    if (step == 0) oled_puts("   ");
    else if (step == 1) oled_puts(".  ");
    else if (step == 2) oled_puts(".. ");
    else if (step == 3) oled_puts("...");
    step++;
    if (step > 3)
      step = 0;
    msleep(500);
  }
  uart_puts(" * Link ok\r\n");
  oled_pos(1, 0);
  oled_puts("Reseau (DHCP)   ");
  
  memset(dhcp_file, 0, 128);
  
  dhcp_session.socket = 2;
  dhcp_session.buffer = (u8 *)buffer;
  DHCP_init(&dhcp_session);
  dhcp_session.dhcp_file = (u8 *)dhcp_file;
  
  uart_puts(" * Request DHCP ");
  
  step = 0;
  while(1)
  {
    dhcp_state = DHCP_run(&dhcp_session);
    if (dhcp_state == DHCP_IP_LEASED)
      break;
    step++;
    if (step == 1250)
    {
      step = 0;
      dhcp_session.tick_1s++;
      uart_putc('.');
    }
  }
  uart_puts(" ok\r\n");
  
  getSIPR(tmp);
  pnt = msg;
  pnt += b2ds(msg, tmp[0]); *pnt++ = '.';
  pnt += b2ds(pnt, tmp[1]); *pnt++ = '.';
  pnt += b2ds(pnt, tmp[2]); *pnt++ = '.';
  pnt += b2ds(pnt, tmp[3]); *pnt = 0;

  uart_puts(" * IP: "); uart_puts(msg); uart_crlf();
      
  oled_pos(1, 0);
  oled_puts("                ");
  oled_pos(1, 0);
  oled_puts(msg);
  
  if (dhcp_file[0] != 0x00)
    ldr_tftp(&dhcp_session);
  else
  {
    uart_puts("ToDo: Update without valid TFTP server\r\n");
    oled_pos(0, 0);
    oled_puts("No TFTP : STOP");
    while(1);
  }
}

void ldr_tftp(dhcp_session *dhcp)
{
  tftp tftp_session;
  int  tftp_block;
  u32  iap_addr;
  int  step;
  
  step = 0;
  while(1)
  {
    if (step == 0)
    {
      tftp_update(dhcp);
      
      tftp_session.port = TFTP_PORT_DEFAULT;
      tftp_init(&tftp_session);
      tftp_session.filename = upd_file;
      tftp_session.server[0] = dhcp->dhcp_siaddr[0];
      tftp_session.server[1] = dhcp->dhcp_siaddr[1];
      tftp_session.server[2] = dhcp->dhcp_siaddr[2];
      tftp_session.server[3] = dhcp->dhcp_siaddr[3];

      uart_puts(" * Erase flash\r\n");
      oled_pos(0, 0);
      oled_puts("Efface flash");
      
      for (iap_addr = 0x8000; iap_addr < 0x20000; iap_addr += 0x1000)
        iap_erase(iap_addr);
      msleep(1000);

      iap_addr = 0x8000;
      
      step ++;
    }
    if (step == 1)
    {
      tftp_run(&tftp_session);
      
      if ( (tftp_session.state == 2) ||
           (tftp_session.state == 3))
      {
        char str[17];
        int len;
        
        if (tftp_session.lastblock != tftp_block)
        {
          tftp_block = tftp_session.lastblock;
          
          strcpy(str, "TFTP: load      ");
          b2ds(&str[11], tftp_block);
          oled_pos(0, 0);
          oled_puts(str);

          len = 0;
          if (tftp_session.length > 4)
          {
            len = tftp_session.length - 4;
            uart_puts("iap_write() "); uart_puthex(iap_addr);
            iap_write(iap_addr, &tftp_session.data[4], len);
            uart_putc('\r');
            iap_addr += len;
          }
        tftp_ack(&tftp_session);
        }
      }
      if (tftp_session.state == 3)
      {
        uart_crlf();
        oled_pos(0, 0);
        oled_puts("TFTP: complet.");
        uart_puts("TFTP: download complete\r\n");
        tftp_session.state = 92;
      }
      if (tftp_session.state == 98)
      {
        oled_pos(0, 0);
        oled_puts("TFTP: timeout!");
        uart_puts("TFTP timeout, restart\r\n");
        tftp_session.timestamp = 0x3000;
        tftp_session.state = 0;
      }
      if (tftp_session.state == 99)
      {
        oled_pos(0, 0);
        oled_puts("TFTP: erreur 1  ");
        step = 2;
      }
      if (tftp_session.state == 92)
      {
        int i;
        uart_puts(" * AutoBoot in 5 sec ");
        for (i = 0; i < 5; i++)
        {
          msleep(500);
          uart_putc(' ');
          msleep(500);
          uart_putc('.');
        }
        uart_crlf();
        NVIC_SystemReset();
      }
    }
  }
}

