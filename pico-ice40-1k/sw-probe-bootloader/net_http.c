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
#include "net_http.h"
#include "types.h"
#include "libc.h"
#include "W7500x_wztoe.h"
#include "uart.h"

static void http_process(http_socket *socket);
static void http_recv_header(http_socket *socket);

void http_init(http_server *server)
{
  http_socket *s;
  
  s = server->socks;
  while(s)
  {
    uart_puts("http_init() Socket "); uart_puthex(s->id); uart_puts("\r\n");
    
    /* Configure socket as TCP */
    setSn_MR  (s->id, Sn_MR_TCP);
    /* Set local port */
    setSn_PORT(s->id, server->port);
    
    s = s->next;
  }
  return;
}

void http_run(http_server *server)
{
  u32 status;
  http_socket *s;
  
  s = server->socks;
  
  while(s)
  {
    status = getSn_SR(s->id);
  
    switch(status)
    {
      case SOCK_CLOSED:
        HTTP_DBG("http_run() SOCK_CLOSED\r\n");
        /* Open the socket */
        setSn_CR  (s->id, Sn_CR_OPEN);
        while( getSn_CR(s->id) )
          ;
          /* Wait until socket is really opened */
        while(getSn_SR(s->id) == SOCK_CLOSED)
          ;
        /* Reset the state-machine */
        s->state   = HTTP_STATE_WAIT;
        s->method  = HTTP_METHOD_NONE;
        s->handler = 0;
        s->tx_len  = 0;
        s->content_len  = 0;
        s->content_priv = 0;
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
        setSn_ICR(s->id, 0x01);
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
    /* Look at the next socket */
    s = s->next;
  }
}

static void http_process(http_socket *socket)
{
  u32  addr;
  u32  offset;
  vu8 *pkt;
  int  len;
  
  len = 0;
  
  /* Search RX data into FIFO */
  offset = getSn_RX_RD(socket->id);
  offset &= 0x7FF; /* Move to the first memory frame */
  addr = WZTOE_RX | (socket->id << 18);
  pkt  = (vu8 *)(addr + offset);
  /* Save RX data address */
  socket->rx = pkt;
  
  /* Search TX data into FIFO */
  offset = (getSn_TX_WR(socket->id) & 0x0FFF);
  addr = WZTOE_TX | (socket->id << 18);
  pkt = (vu8 *)(addr + offset);
  /* Save TX data address */
  socket->tx = pkt;
  
  if (socket->state == HTTP_STATE_WAIT)
  {
    len = getSn_RX_RSR(socket->id);
    if (len == 0)
      return;
    HTTP_DBG("> http_process() HTTP_STATE_WAIT\r\n");
    socket->rx_len = len;
    http_recv_header(socket);
  }
  
  if (socket->state == HTTP_STATE_ERROR)
  {
    HTTP_DBG("http_process() HTTP_STATE_ERROR\r\n");
    setSn_CR(socket->id, Sn_CR_DISCON);
    while( getSn_CR(socket->id) )
      ;
  }
  
  if (socket->state == HTTP_STATE_SEND_MORE)
  {
    HTTP_DBG("> http_process() HTTP_STATE_SEND_MORE\r\n");
    /* Wait end of the previous packet */
    while(getSn_TX_FSR(socket->id) < (getSn_TXBUF_SIZE(socket->id)*1024))
      ;
    /* Call CGI (again) */
    socket->handler->cgi(socket);
  }
  
  if (socket->state == HTTP_STATE_RECV_MORE)
  {
    len = getSn_RX_RSR(socket->id);
    if (len == 0)
      return;
    HTTP_DBG("> http_process() HTTP_STATE_RECV_MORE\r\n");
    socket->rx_len = len;
    /* Call CGI (again) */
    socket->handler->cgi(socket);
  }
  
  if (socket->state == HTTP_STATE_REQUEST)
  {
    HTTP_DBG("> http_process() HTTP_STATE_REQUEST\r\n");
    
    if (socket->handler && socket->handler->cgi)
      socket->handler->cgi(socket);
  }
  
  if (len)
  {
    /* Update RX pointer */
    offset = getSn_RX_RD(socket->id);
    offset += len;
    offset &= 0xFFFF;
    setSn_RX_RD(socket->id, offset);
    /* Mark datas as readed */
    setSn_CR(socket->id, Sn_CR_RECV);
    while( getSn_CR(socket->id) )
      ;
    /* Clear socket variables */
    socket->rx      = 0;
    socket->rx_head = 0;
    socket->rx_len  = 0;    
  }
  
  if (socket->state == HTTP_STATE_NOT_FOUND)
  {
    HTTP_DBG("http_process() HTTP_STATE_NOT_FOUND\r\n");
    if (socket->server->err404)
      socket->content_len = strlen(socket->server->err404);
    else
      socket->content_len = 0;
    http_send_header(socket, 404, HTTP_CONTENT_HTML);
    if (socket->server->err404)
    {
      strcat((char *)socket->tx, socket->server->err404);
      socket->tx_len += strlen(socket->server->err404);
    }
    socket->state = HTTP_STATE_SEND;
  }
  
  if ( (socket->state == HTTP_STATE_SEND)     ||
       (socket->state == HTTP_STATE_SEND_MORE) )
  {
    HTTP_DBG("> http_process() HTTP_STATE_SEND\r\n");
    /* Update TX fifo */
    offset  = getSn_TX_WR(socket->id);
    offset += socket->tx_len;
    offset &= 0xFFFF;
    setSn_TX_WR(socket->id, offset);
    /* Send datas */
    setSn_CR(socket->id, Sn_CR_SEND);
    while( getSn_CR(socket->id) )
      ;
    while(1)
    {
      if (getSn_IR(socket->id) & 0x10)
      {
        setSn_ICR(socket->id, 0x10);
        break;
      }
    }
    /* Reset tx counter */
    socket->tx_len = 0;
    
    if (socket->state == HTTP_STATE_SEND)
    {
      socket->state = HTTP_STATE_WAIT;
      socket->content_priv  = 0;
    }
  }
}

static void http_recv_header(http_socket *socket)
{
  const char *pnt;
  const char *token;
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
  
  if(!strncmp(token, "GET ", 4) || !strncmp(token, "get ", 4))
    socket->method = HTTP_METHOD_GET;
  else if (!strncmp(token, "POST ", 5) || !strncmp(token, "post ", 5))
    socket->method = HTTP_METHOD_POST;
  else
    goto parse_error;
  
  /* 3) Decode the requested URI */
  
  token = (pnt + 1);
  pnt = strchr(token, ' ');
  if (pnt == 0)
    goto parse_error;
  
  /* 4) Search a valid handler for the requested content */
  content = socket->server->contents;
  while (content)
  {
    int len;
    int cmp;
    
    len = strlen(content->name);
    cmp = strncmp(token, content->name, len);
    
    if (cmp == 0)
    {
      /* If wildcard is set, sufficient */
      if (content->wildcard)
        break;
      
      /* Search for a space separator at the end */
      if (token[len] == ' ')
        break;
    }
    content = content->next;
  }
  if (content == 0)
  {
    socket->state = HTTP_STATE_NOT_FOUND;
    goto parse_exit;
  }
  
  socket->handler = content;
  socket->uri     = (char *)token;
  
  /* Search the end of line */
  pnt = strchr(pnt + 1, 0x0A);
  if (pnt)
    rx_head = (u8 *)(pnt + 1);
  
  socket->rx_len -= (rx_body - socket->rx);
  socket->rx_head = rx_head;
  socket->rx      = rx_body;
  socket->state   = HTTP_STATE_REQUEST;
  return;
  
parse_error:
  /* DEBUG uart_puts((char *)socket->rx); */
  socket->state = HTTP_STATE_ERROR;
parse_exit:
  return;
}

void http_send_header(http_socket *socket, int code, int type)
{
  char buffer[8];
  vu8 *pkt;
  int  len;
  
  pkt = socket->tx;
  
  if (code == 200)
    strcpy((char *)pkt, "HTTP/1.1 200 OK\r\n");
  else if (code == 303)
    strcpy((char *)pkt, "HTTP/1.1 303 See Other\r\n");
  else
    strcpy((char *)pkt, "HTTP/1.1 404 Not Found\r\n");
  
  len = b2ds(buffer, socket->content_len);
  buffer[len] = 0;
  
  strcat((char *)pkt, "Content-Type: ");
  switch(type)
  {
    case HTTP_CONTENT_HTML: strcat((char *)pkt, "text/html\r\n");  break;
    case HTTP_CONTENT_CSS:  strcat((char *)pkt, "text/css\r\n");   break;
    case HTTP_CONTENT_PNG:  strcat((char *)pkt, "image/png\r\n");  break;
    case HTTP_CONTENT_JPEG: strcat((char *)pkt, "image/jpeg\r\n"); break;
    default:
      strcat((char *)pkt, "text/plain\r\n");
  }
  strcat((char *)pkt, "Connection: keep-alive\r\n");
  /* Add the content length */
  strcat((char *)pkt, "Content-Length: ");
  strcat((char *)pkt, buffer);
  strcat((char *)pkt, "\r\n");
  /* Add an empty line (end of header) */
  strcat((char *)pkt, "\r\n");
  
  len = strlen((char *)pkt);
  
  socket->tx     += len;
  socket->tx_len += len;
}

char *http_get_header(http_socket *socket, char *p)
{
  char *line;
  
  if (p == 0)
    return((char *)socket->rx_head);
  
  line = strchr(p, 0x0d);
  if (line == 0)
    return(0);
    
  line += 2;
  return(line);
}
/* EOF */
