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

int cgi_spi(http_socket *socket)
{
  u8 rd;
  u8 wr;
  u8 *pnt;
  
  uart_puts("main::cgi_spi() ");
  
  pnt = (u8 *)socket->rx;
  pnt = (u8 *)strchr((char *)pnt, '=');
  uart_puts((char*)pnt); uart_puts("  ");
  if (pnt == 0)
    goto cgi_error;
  
  {  
    pnt++;
    if ((*pnt >= '0') && (*pnt <= '9'))
      wr = (*pnt - '0') << 4;
    else if ((*pnt >= 'A') && (*pnt <= 'F'))
      wr = ((*pnt - 'A') + 10) << 4;
    else if ((*pnt >= 'a') && (*pnt <= 'f'))
      wr = ((*pnt - 'a') + 10) << 4;
    
    pnt++;
    if ((*pnt >= '0') && (*pnt <= '9'))
      wr |= (*pnt - '0');
    else if ((*pnt >= 'A') && (*pnt <= 'F'))
      wr |= ((*pnt - 'A') + 10);
    else if ((*pnt >= 'a') && (*pnt <= 'f'))
      wr |= ((*pnt - 'A') + 10);
  }
  
  pld_cs(1);
  spi_wr(wr);
  rd = spi_rd();
  pld_cs(0);
  
  uart_puthex8(rd); uart_puts("\r\n");
  
//  i = b2ds(buf, rd);
//  buf[i] = 0;
//  *len = i;

  socket->content_len = 15;
  http_send_header(socket, 200, 0);
  strcpy((char *)socket->tx, "{\"result\":\"ok\"}");
  socket->tx_len += 15;
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

