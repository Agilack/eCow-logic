/**
 * eCow-logic - Embedded probe main firmware
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
#include "net_http.h"
#include "types.h"
#include "libc.h"
#include "W7500x_wztoe.h"
#include "uart.h"

static void http_recv(void);
static void http_send_header(void);

void http_init(void)
{
  /* Configure socket as TCP */
  setSn_MR  (HTTP_NG_SOCK, Sn_MR_TCP);
  /* Set local port */
  setSn_PORT(HTTP_NG_SOCK, 808);
  
  return;
}

void http_run(void)
{
  u32 status;
  
  status = getSn_SR(HTTP_NG_SOCK);
  
  switch(status)
  {
    case SOCK_CLOSED:
      /* Open the socket */
      setSn_CR  (HTTP_NG_SOCK, Sn_CR_OPEN);
      while( getSn_CR(HTTP_NG_SOCK) )
        ;
      break;
    
    case SOCK_INIT:
      /* Set socket to LISTEN mode */
      setSn_CR(HTTP_NG_SOCK, Sn_CR_LISTEN);
      while( getSn_CR(HTTP_NG_SOCK) )
        ;
      /* Wait until socket initialisation complete */
      while(getSn_SR(HTTP_NG_SOCK) != SOCK_LISTEN)
        ;
      break;
    
    case SOCK_LISTEN:
      break;
    
    case SOCK_ESTABLISHED:
      http_recv();
      break;
    
    case SOCK_CLOSE_WAIT:
      /* Disconnect the socket */
      setSn_CR(HTTP_NG_SOCK, Sn_CR_DISCON);
      while( getSn_CR(HTTP_NG_SOCK) )
        ;
      /* Wait until the socket is closed */
      while(getSn_SR(HTTP_NG_SOCK) != SOCK_CLOSED)
        ;
      break;      
  }
}

static void http_recv(void)
{
  u32 addr;
  u32 offset;
  u8 *pkt;
  int len;
  int i;
  
  len = getSn_RX_RSR(HTTP_NG_SOCK);
  if (len == 0)
    return;
  offset = (getSn_RX_RD(HTTP_NG_SOCK) & 0xFFF);
  addr = WZTOE_RX | (HTTP_NG_SOCK << 18);
  pkt  = (u8 *)(addr + offset);
  
  uart_puts("\r\n");
  for (i = 0; i < len; i++)
  {
    uart_putc(pkt[i]);
  }
  uart_puts("\r\n");
  
  /* Update RX pointer */
  offset += len;
  setSn_RX_RD(HTTP_NG_SOCK, offset);
  /* Mark datas as readed */
  setSn_CR(HTTP_NG_SOCK, Sn_CR_RECV);
  while( getSn_CR(HTTP_NG_SOCK) )
    ;
  
  http_send_header();
}

static void http_send_header(void)
{
  u32 addr;
  u32 offset;
  u8 *pkt;
  int len;
  
  offset = getSn_TX_RD(HTTP_NG_SOCK);
  offset &= 0x0FFF;
  setSn_TX_RD(HTTP_NG_SOCK, offset);
  
  offset = (getSn_TX_WR(HTTP_NG_SOCK) & 0x0FFF);
  addr = WZTOE_TX | (HTTP_NG_SOCK << 18);
  pkt = (u8 *)(addr + offset);
  
  //strcpy((char *)pkt, "HTTP/1.1 200 OK\r\n");
  strcpy((char *)pkt, "HTTP/1.1 404 Not Found\r\n");
  strcat((char *)pkt, "Content-Type: text/html\r\n");
  strcat((char *)pkt, "Connection: close\r\n");
  strcat((char *)pkt, "Content-Length: 78\r\n");
  strcat((char *)pkt, "\r\n");
  
  strcat((char *)pkt, "<HTML>\r\n<BODY>\r\nSorry, the page you requested was not found.\r\n</BODY>\r\n</HTML>\r\n\0");
  
  len = strlen((char *)pkt);
  uart_puts("Send "); uart_puthex(len); uart_puts("\r\n");
  
  /* Send datas */
  offset += len;
  setSn_TX_WR(HTTP_NG_SOCK, offset);
  setSn_CR(HTTP_NG_SOCK, Sn_CR_SEND);
  while( getSn_CR(HTTP_NG_SOCK) )
    ;
}
/* EOF */
