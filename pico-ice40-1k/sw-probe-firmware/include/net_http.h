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
#ifndef HTTP_H
#define HTTP_H

#define HTTP_STATE_WAIT       0
#define HTTP_STATE_REQUEST    1
#define HTTP_STATE_RECV_MORE  2
#define HTTP_STATE_SEND       3
#define HTTP_STATE_SEND_MORE  4
#define HTTP_STATE_NOT_FOUND 98
#define HTTP_STATE_ERROR     99

#define HTTP_METHOD_NONE 0
#define HTTP_METHOD_GET  1
#define HTTP_METHOD_POST 2

#define HTTP_CONTENT_PLAIN    0
#define HTTP_CONTENT_HTML     1
#define HTTP_CONTENT_CSS      2
#define HTTP_CONTENT_PNG      3
#define HTTP_CONTENT_JPEG     4

#ifdef DEBUG
#define HTTP_DBG(x)    uart_puts(x)
#else
#define HTTP_DBG(x)
#endif

struct _http_content;
struct _http_socket;

typedef struct _http_server
{
  int port;
  struct _http_content *contents;
  struct _http_socket  *socks;
} http_server;

typedef struct _http_socket
{
  int   id;
  int   state;
  int   method;
  char *uri;
  struct _http_content *handler;
  int   content_len;
  void *content_priv;
  u8   *rx;
  u32   rx_len;
  u8   *rx_head;
  u8   *tx;
  u32   tx_len;
  struct _http_server *server;
  struct _http_socket *next;
} http_socket;

typedef struct _http_content
{
  char name[16];
  int  wildcard;
  int  (*cgi)(http_socket *socket);
  struct _http_content *next;
} http_content;

extern void (*http_init)(http_server *server);
extern void (*http_run) (http_server *server);
extern void (*http_send_header)(http_socket *socket, int code, int type);
extern char *(*http_get_header)(http_socket *socket, char *p);

#endif
/* EOF */
