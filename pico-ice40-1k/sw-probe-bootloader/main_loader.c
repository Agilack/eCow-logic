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
#include "iap.h"
#include "libc.h"
#include "miim.h"
#include "oled.h"
#include "uart.h"
#include "net_http.h"
#include "net_tftp.h"
#include "update.h"

static void ldr_tftp(dhcp_session *dhcp);
static void ldr_http(dhcp_session *dhcp);
static int  ldr_cgi (http_socket  *socket);

void main_loader(void)
{
  char buffer[2048];
  char msg[16];
  char tmp[4];
  char dhcp_file[128];
  u8 dhcp_state;
  dhcp_session dhcp_session;
  int  step;
  char *pnt;
  
  uart_puts(" * Start LOADER mode \r\n");
  
  oled_pos(3, 0);
  oled_puts("eCowL     Loader");

  /* PHY Link Check via gpio mdio */
  oled_pos(1, 0);
  oled_puts("Init reseau ...");
  step = 0;
  while( link() == 0x0 )
  {
    uart_putc('.');
    oled_pos(1, 13);
    if (step == 0) oled_puts("   ");
    else if (step == 1) oled_puts(".  ");
    else if (step == 2) oled_puts(".. ");
    else if (step == 3) oled_puts("...");
    step++;
    if (step > 3)
      step = 0;
    msleep(500);
  }
  uart_puts(" * Link ok\r\n");
  oled_pos(1, 0);
  oled_puts("Reseau (DHCP)   ");
  
  memset(dhcp_file, 0, 128);
  
  dhcp_session.socket = 2;
  dhcp_session.buffer = (u8 *)buffer;
  DHCP_init(&dhcp_session);
  dhcp_session.dhcp_file = (u8 *)dhcp_file;
  
  uart_puts(" * Request DHCP ");
  
  step = 0;
  while(1)
  {
    dhcp_state = DHCP_run(&dhcp_session);
    if (dhcp_state == DHCP_IP_LEASED)
      break;
    step++;
    if (step == 1250)
    {
      step = 0;
      dhcp_session.tick_1s++;
      uart_putc('.');
    }
  }
  uart_puts(" ok\r\n");
  
  getSIPR(tmp);
  pnt = msg;
  pnt += b2ds(msg, tmp[0]); *pnt++ = '.';
  pnt += b2ds(pnt, tmp[1]); *pnt++ = '.';
  pnt += b2ds(pnt, tmp[2]); *pnt++ = '.';
  pnt += b2ds(pnt, tmp[3]); *pnt = 0;

  uart_puts(" * IP: "); uart_puts(msg); uart_crlf();
      
  oled_pos(1, 0);
  oled_puts("                ");
  oled_pos(1, 0);
  oled_puts(msg);
  
  if (dhcp_file[0] != 0x00)
    ldr_tftp(&dhcp_session);
  else
    ldr_http(&dhcp_session);
  
  while(1);
}

static void ldr_tftp(dhcp_session *dhcp)
{
  tftp tftp_session;
  int  tftp_block;
  u32  iap_addr;
  int  step;
  
  step = 0;
  while(1)
  {
    if (step == 0)
    {
      tftp_update(dhcp);
      
      tftp_session.port = TFTP_PORT_DEFAULT;
      tftp_init(&tftp_session);
      tftp_session.filename = (const char *)dhcp->dhcp_file;
      tftp_session.server[0] = dhcp->dhcp_siaddr[0];
      tftp_session.server[1] = dhcp->dhcp_siaddr[1];
      tftp_session.server[2] = dhcp->dhcp_siaddr[2];
      tftp_session.server[3] = dhcp->dhcp_siaddr[3];

      uart_puts(" * Erase flash\r\n");
      oled_pos(0, 0);
      oled_puts("Efface flash");
      
      for (iap_addr = 0x8000; iap_addr < 0x20000; iap_addr += 0x1000)
        iap_erase(iap_addr);
      msleep(1000);

      iap_addr = 0x8000;
      
      step ++;
    }
    if (step == 1)
    {
      tftp_run(&tftp_session);
      
      if ( (tftp_session.state == 2) ||
           (tftp_session.state == 3))
      {
        char str[17];
        int len;
        
        if (tftp_session.lastblock != tftp_block)
        {
          tftp_block = tftp_session.lastblock;
          
          strcpy(str, "TFTP: load      ");
          b2ds(&str[11], tftp_block);
          oled_pos(0, 0);
          oled_puts(str);

          len = 0;
          if (tftp_session.length > 4)
          {
            len = tftp_session.length - 4;
            uart_puts("iap_write() "); uart_puthex(iap_addr);
            iap_write(iap_addr, &tftp_session.data[4], len);
            uart_putc('\r');
            iap_addr += len;
          }
        tftp_ack(&tftp_session);
        }
      }
      if (tftp_session.state == 3)
      {
        uart_crlf();
        oled_pos(0, 0);
        oled_puts("TFTP: complet.");
        uart_puts("TFTP: download complete\r\n");
        tftp_session.state = 92;
      }
      if (tftp_session.state == 98)
      {
        oled_pos(0, 0);
        oled_puts("TFTP: timeout!");
        uart_puts("TFTP timeout, restart\r\n");
        tftp_session.timestamp = 0x3000;
        tftp_session.state = 0;
      }
      if (tftp_session.state == 99)
      {
        oled_pos(0, 0);
        oled_puts("TFTP: erreur 1  ");
        step = 2;
      }
      if (tftp_session.state == 92)
      {
        int i;
        uart_puts(" * AutoBoot in 5 sec ");
        for (i = 0; i < 5; i++)
        {
          msleep(500);
          uart_putc(' ');
          msleep(500);
          uart_putc('.');
        }
        uart_crlf();
        NVIC_SystemReset();
      }
    }
  }
}

static void ldr_http(dhcp_session *dhcp)
{
  http_server  http;
  http_socket  httpsock;
  http_content httpcontent;
  
  oled_pos(2, 0);
  oled_puts("Mode HTTP");

  /* Init HTTP content */
  strcpy(httpcontent.name, "/");
  httpcontent.wildcard = 1;
  httpcontent.cgi      = ldr_cgi;
  httpcontent.next     = 0;
  /* Init HTTP socket */
  httpsock.id     = 4;
  httpsock.state  = 0;
  httpsock.server = &http;  
  httpsock.next   = 0;
  /* Init HTTP server */
  http.port     = 80;
  http.socks    = &httpsock;
  http.contents = &httpcontent; 
  http_init(&http);
  
  while(1)
  {
    http_run(&http); 
  }
}

const char cgi_content[] = 
  "<html>"
    "<h1>eCowLogic Bootloader</h1>"
    "<h3>Firmware update</h3>"
    "<form method=\"post\" action=\"/fw\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"bit\" />"
      "<input type=\"submit\" />"
    "</form>"
    "<h3>External flash update</h3>"
    "<form method=\"post\" action=\"/flash\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"mem\" />"
      "<input type=\"submit\" />"
    "</form>"
  "</html>";

static int ldr_cgi(http_socket *socket)
{
  char *file;
  int   mph_len;
  int   len;
  char *pnt;
  u32   content_length;
  int   i;
  
  uart_puts("ldr_cgi()\r\n");
  
  if (socket->method == HTTP_METHOD_POST)
  {
    if (socket->content_priv == 0)
    {
      content_length = 0;
      
      pnt = 0;
      for (i = 0; i < 16; i++)
      {
        char arg[16];
        pnt = http_get_header(socket, pnt);
        if (pnt == 0)
          break;
        if (strncmp(pnt, "Content-Length:", 15) == 0)
        {
          pnt += 16;
          for (i = 0; i < 7; i++)
          {
            if ( (*pnt < '0') || (*pnt > '9') )
              break;
            arg[i] = *pnt;
            pnt++;
          }
          arg[i] = 0;
          content_length = atoi(arg);
          break;
        }
      }
      uart_puts("file length=");
      uart_puthex(content_length);
      uart_puts("\r\n");
      
      pnt = (char *)socket->rx;
      while(pnt != 0)
      {
        if ( (pnt[0] == 0x0d) && (pnt[1] == 0x0a) &&
             (pnt[2] == 0x0d) && (pnt[3] == 0x0a) )
        {
      	  /* Get a pointer on datas (after multipart header) */
      	  file = (pnt + 4);
      	  break;
        }
        /* Search the next CR */
        pnt = strchr(pnt + 1, 0x0d);
      }
      /* Multipart header length */
      mph_len = ((u32)file - (u32)socket->rx);
      /* Received length */
      len = socket->rx_len - mph_len;
      
      /* ToDo : Write datas to memory ... */
      
      uart_puts("len = "); uart_puthex(len); uart_puts("\r\n");
      
      if (len < content_length)
        socket->state = HTTP_STATE_RECV_MORE;
      /* Save the size of remaining datas */
      socket->content_priv = (void *)(content_length - mph_len - len);
    }
    /* Else, socket private data is not null */
    else
    {
      content_length = (u32)socket->content_priv;
      uart_puts("Wait for ");
      uart_puthex(content_length); uart_puts(" bytes, received ");
      uart_puthex(socket->rx_len); uart_puts("\r\n");
      
      /* ToDo : Write datas to memory ... */
      
      if (socket->rx_len < content_length)
        socket->content_priv = (void *)(content_length - socket->rx_len);
      else
      {
        uart_puts("Transfer complete.\r\n");
        socket->content_priv = 0;
        socket->content_len = 0;
        http_send_header(socket, 200, 0);
        socket->state = HTTP_STATE_SEND;
      }
    }
    return(0);
  }
  
  socket->content_len = strlen(cgi_content);
  http_send_header(socket, 200, HTTP_CONTENT_HTML);
  
  strcpy((char *)socket->tx, cgi_content);
  socket->tx_len += strlen(cgi_content);
  
  socket->state = HTTP_STATE_SEND;
  return(0);
}
/* EOF */
