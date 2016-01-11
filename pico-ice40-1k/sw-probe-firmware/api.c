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
#include "types.h"
#include "flash_fs.h"
#include "net_dhcp.h"
#include "net_tftp.h"

void (*oled_line) (int n);
void (*oled_puts) (char *s);

void (*dhcp_init)(dhcp *session);
int  (*dhcp_run) (dhcp *session);

void (*tftp_init)(tftp *session);
int  (*tftp_run) (tftp *session);
void (*tftp_ack) (tftp *session);
void (*tftp_stop)(tftp *session);

void (*uart_putc)    (u8);
void (*uart_puts)    (char *);
void (*uart_puthex8) (const u8 c);

void (*flash_read) (u32 addr, u8 *buffer, u32 len);
int  (*fs_getentry)(int n, fs_entry *entry);

void api_init(void)
{
  dhcp_init = (void (*)(dhcp *)) *(u32 *)0x000000B0;
  dhcp_run  = (int  (*)(dhcp *)) *(u32 *)0x000000B4; 
  
  tftp_init = (void (*)(tftp *)) *(u32 *)0x000000B8;
  tftp_run  = (int  (*)(tftp *)) *(u32 *)0x000000BC; 
  tftp_ack  = (void (*)(tftp *)) *(u32 *)0x000000C0; 
  tftp_stop = (void (*)(tftp *)) *(u32 *)0x000000C4;
  
  oled_line = (void (*)(int))    *(u32 *)0x000000D4;
  oled_puts = (void (*)(char *)) *(u32 *)0x000000D8;
  
  uart_putc    = (void (*)(u8))       *(u32 *)0x000000E0;
  uart_puts    = (void (*)(char *))   *(u32 *)0x000000E4;
  uart_puthex8 = (void (*)(const u8)) *(u32 *)0x000000E8;
  
  flash_read   = (void (*)(u32,u8*,u32))   *(u32 *)0x000000F0;
  fs_getentry  = (int  (*)(int,fs_entry*)) *(u32 *)0x000000FC;
}
/* EOF */