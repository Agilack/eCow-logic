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
#include "net_tftp.h"

void (*oled_line) (int n);
void (*oled_puts) (char *s);

void (*tftp_init)(tftp *session);
int  (*tftp_run) (tftp *session);

void (*uart_putc)    (u8);
void (*uart_puts)    (char *);
void (*uart_puthex8) (const u8 c);

void api_init(void)
{
  tftp_init = (void (*)(tftp *)) *(u32 *)0x000000B8;
  tftp_run  = (int  (*)(tftp *)) *(u32 *)0x000000BC; 
  
  oled_line = (void (*)(int))    *(u32 *)0x000000C4;
  oled_puts = (void (*)(char *)) *(u32 *)0x000000C8;
  
  uart_putc    = (void (*)(u8))       *(u32 *)0x000000D0;
  uart_puts    = (void (*)(char *))   *(u32 *)0x000000D4;
  uart_puthex8 = (void (*)(const u8)) *(u32 *)0x000000D8;
}
/* EOF */