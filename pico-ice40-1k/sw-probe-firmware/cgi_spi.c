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
#include "uart.h"
#include "spi.h"
#include "pld.h"
#include "net_http.h"
#include "libc.h"

static u8 cgi_spi_rw(u8 tx, u8 *txt);

int cgi_spi(http_socket *socket)
{
  char rd[128];
  u8 val;
  u8 *pnt, *lastp, *end;
  char *prd;
  
  uart_puts("main::cgi_spi()\r\n");
  
  pnt = (u8 *)socket->rx;
  pnt = (u8 *)strchr((char *)pnt, '=');
  if (pnt == 0)
    goto cgi_error;
  pnt++;
  
  end = (u8 *)socket->rx + socket->rx_len;
  
  uart_puts("TX : ");
  
  val = 0;
  prd = rd;
  lastp = pnt;
  while(*pnt && (pnt < end))
  {
    if ((*pnt >= '0') && (*pnt <= '9'))
      val = (val << 4) | (*pnt - '0');
    else if ((*pnt >= 'A') && (*pnt <= 'F'))
      val = (val << 4) | ((*pnt - 'A') + 10);
    else if ((*pnt >= 'a') && (*pnt <= 'f'))
      val = (val << 4) | ((*pnt - 'a') + 10);
    
    else if (*pnt == ' ')
    {
      uart_puthex8(val); uart_putc(' ');
      cgi_spi_rw(val, (u8 *)prd);
      prd += strlen(prd);
      lastp = pnt;
      val = 0;
    }
    
    pnt++;
  }
  if (lastp != pnt)
  {
    uart_puthex8(val); uart_putc(' ');
    cgi_spi_rw(val, (u8 *)prd);
    prd += strlen(prd);
  }
  
  uart_puts("\r\nRX : ");
  uart_puts(rd); uart_puts("\r\n");
  
  socket->content_len = strlen(rd);
  http_send_header(socket, 200, 0);
  strcpy((char *)socket->tx, rd);
  socket->tx_len += strlen(rd);
  socket->state = HTTP_STATE_SEND;

  return 0;
  
cgi_error:
  socket->content_len = 15;
  http_send_header(socket, 200, 0);
  strcpy((char *)socket->tx, "{\"result\":\"ko\"}");
  socket->tx_len += 15;
  socket->state = HTTP_STATE_SEND;
  return 0;
}

static u8 cgi_spi_rw(u8 tx, u8 *txt)
{
  const char hex[16] = "0123456789ABCDEF";
  u8 rx;
  
  pld_cs(1);
  spi_wr(tx);
  rx = spi_rd();
  pld_cs(0);
  
  if (txt)
  {
    *txt = hex[ (rx >> 4) & 0x0F];
    txt++;
    *txt = hex[rx & 0x0F];
    txt++;
    *txt = ' ';
    txt++;
    *txt = 0;
  }
  return(rx);
}
/* EOF */
