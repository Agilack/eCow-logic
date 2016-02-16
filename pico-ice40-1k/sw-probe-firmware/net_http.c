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

static void http_process(http_socket *socket);
static void http_recv_header(http_socket *socket);
static void http_send_header(http_socket *socket);

void http_init(http_server *server)
{
  http_socket *s;
  
  s = server->socks;
  
  /* Configure socket as TCP */
  setSn_MR  (s->id, Sn_MR_TCP);
  /* Set local port */
  setSn_PORT(s->id, server->port);
  
  return;
}

void http_run(http_server *server)
{
  u32 status;
  http_socket *s;
  
  s = server->socks;
  
  status = getSn_SR(s->id);
  
  switch(status)
  {
    case SOCK_CLOSED:
      /* Open the socket */
      setSn_CR  (s->id, Sn_CR_OPEN);
      while( getSn_CR(s->id) )
        ;
      /* Reset the state-machine */
      s->state   = HTTP_STATE_WAIT;
      s->method  = HTTP_METHOD_NONE;
      s->handler = 0;
      break;
    
    case SOCK_INIT:
      /* Set socket to LISTEN mode */
      setSn_CR(s->id, Sn_CR_LISTEN);
      while( getSn_CR(s->id) )
        ;
      /* Wait until socket initialisation complete */
      while(getSn_SR(s->id) != SOCK_LISTEN)
        ;
      break;
    
    case SOCK_LISTEN:
      break;
    
    case SOCK_ESTABLISHED:
      http_process(s);
      break;
    
    case SOCK_CLOSE_WAIT:
      /* Disconnect the socket */
      setSn_CR(s->id, Sn_CR_DISCON);
      while( getSn_CR(s->id) )
        ;
      /* Wait until the socket is closed */
      while(getSn_SR(s->id) != SOCK_CLOSED)
        ;
      break;      
  }
}

static void http_process(http_socket *socket)
{
  u32 addr;
  u32 offset;
  u8 *pkt;
  int len;
  
  len = getSn_RX_RSR(socket->id);
  if (len == 0)
    return;
  offset = (getSn_RX_RD(socket->id) & 0xFFF);
  addr = WZTOE_RX | (socket->id << 18);
  pkt  = (u8 *)(addr + offset);
  
  socket->rx = pkt;
  
  if (socket->state == HTTP_STATE_WAIT)
    http_recv_header(socket);
  
  if (socket->state == HTTP_STATE_REQUEST)
  {
    uart_puts("http_process() HTTP_STATE_REQUEST\r\n");   
  }
  
  /* Update RX pointer */
  offset += len;
  setSn_RX_RD(socket->id, offset);
  /* Mark datas as readed */
  setSn_CR(socket->id, Sn_CR_RECV);
  while( getSn_CR(socket->id) )
    ;
  
  http_send_header(socket);
}

static void http_recv_header(http_socket *socket)
{
  char *pnt;
  char *token;
  u8   *rx_body = 0;
  u8   *rx_head = 0;
  http_content *content;
  
  /* 1) Search the end of the header */
  pnt = (char *)socket->rx;
  while(pnt)
  {
    if ( (pnt[0] == 0x0d) && (pnt[1] == 0x0a) &&
         (pnt[2] == 0x0d) && (pnt[3] == 0x0a) )
    {
      /* Get a pointer on HTTP body (after header) */
      rx_body = (u8 *)(pnt + 4);
      /* Cut the string between header and body */
      pnt[0] = 0;
      break;
    }
    /* Search the next CR */
    pnt = strchr(pnt + 1, 0x0d);
  }
  /* If no cr-lf-cr-lf found, bad header */
  if (pnt == 0)
    goto parse_error;
  
  /* 2) Decode the requested Method */
  token = (char *)socket->rx;
  pnt = strchr(token, ' ');
  if (pnt == 0)
    goto parse_error;
  *pnt = 0; /* Cut the string */
  
  if(!strcmp(token, "GET") || !strcmp(token, "get"))
    socket->method = HTTP_METHOD_GET;
  else if (!strcmp(token, "POST") || !strcmp(token, "post"))
    socket->method = HTTP_METHOD_POST;
  else
    goto parse_error;
  
  /* 3) Decode the requested URI */
  
  token = (pnt + 1);
  pnt = strchr(pnt, ' ');
  if (pnt == 0)
    goto parse_error;
  *pnt = 0; /* Cut the string */
  
  /* 4) Search a valid handler for the requested content */
  content = socket->server->contents;
  while (content)
  {
    if (content->wildcard)
    {
      if (strncmp(token, content->name, strlen(content->name)) == 0)
        break;
    }
    else
    {
      if (strcmp(token, content->name) == 0)
        break;
    }
    content = content->next;
  }
  socket->handler = content;
  
  /* Search the end of line */
  pnt = strchr(pnt + 1, 0x0A);
  if (pnt)
    rx_head = (u8 *)(pnt + 1);
  
  socket->rx_head = rx_head;
  socket->rx      = rx_body;
  socket->state   = HTTP_STATE_REQUEST;
  return;
  
parse_error:
  socket->state = HTTP_STATE_ERROR;
  return;
}

static void http_send_header(http_socket *socket)
{
  u32 addr;
  u32 offset;
  u8 *pkt;
  int len;
  
  offset = getSn_TX_RD(socket->id);
  offset &= 0x0FFF;
  setSn_TX_RD(socket->id, offset);
  
  offset = (getSn_TX_WR(socket->id) & 0x0FFF);
  addr = WZTOE_TX | (socket->id << 18);
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
  setSn_TX_WR(socket->id, offset);
  setSn_CR(socket->id, Sn_CR_SEND);
  while( getSn_CR(socket->id) )
    ;
}
/* EOF */
