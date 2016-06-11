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
#include "net_http.h"

void (*oled_pos)  (int y, int x);
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

void  (*http_init) (http_server *server);
void  (*http_run)  (http_server *server);
char *(*http_get_header) (http_socket *socket, char *p);
void  (*http_send_header)(http_socket *socket, int code, int type);

void api_init(void)
{
  dhcp_init = (void (*)(dhcp *)) *(u32 *)0x000000B0;
  dhcp_run  = (int  (*)(dhcp *)) *(u32 *)0x000000B4; 
  
  tftp_init = (void (*)(tftp *)) *(u32 *)0x000000C0;
  tftp_run  = (int  (*)(tftp *)) *(u32 *)0x000000C4; 
  tftp_ack  = (void (*)(tftp *)) *(u32 *)0x000000C8; 
  tftp_stop = (void (*)(tftp *)) *(u32 *)0x000000CC;
  
  oled_pos  = (void (*)(int,int)) *(u32 *)0x000000D4;
  oled_puts = (void (*)(char *))  *(u32 *)0x000000D8;
  
  uart_putc    = (void (*)(u8))       *(u32 *)0x000000E0;
  uart_puts    = (void (*)(char *))   *(u32 *)0x000000E4;
  uart_puthex8 = (void (*)(const u8)) *(u32 *)0x000000E8;
  
  flash_read   = (void (*)(u32,u8*,u32))   *(u32 *)0x000000F0;
  fs_getentry  = (int  (*)(int,fs_entry*)) *(u32 *)0x000000FC;
  
  http_init    = (void (*)(http_server *)) *(u32 *)0x00000110;
  http_run     = (void (*)(http_server *)) *(u32 *)0x00000114;
  http_get_header = (char *(*)(http_socket *,char *))  *(u32 *)0x00000118;
  http_send_header= (void  (*)(http_socket *,int,int)) *(u32 *)0x0000011C;
}
/* EOF */