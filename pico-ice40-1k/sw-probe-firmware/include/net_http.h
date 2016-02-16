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
#ifndef HTTP_H
#define HTTP_H

#define HTTP_STATE_WAIT     0
#define HTTP_STATE_REQUEST  1
#define HTTP_STATE_ERROR   99
#define HTTP_METHOD_NONE 0
#define HTTP_METHOD_GET  1
#define HTTP_METHOD_POST 2

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
  struct _http_content *handler;
  u8   *rx;
  u8   *rx_head;
  struct _http_server *server;
} http_socket;

typedef struct _http_content
{
  char name[16];
  int  wildcard;
  int  (*cgi)(http_socket *socket);
  struct _http_content *next;
} http_content;

void http_init(http_server *server);
void http_run (http_server *server);

#endif
/* EOF */
