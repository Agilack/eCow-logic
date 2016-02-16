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
#include "net_dhcp.h"
#include "flash.h"
#include "flash_fs.h"
#include "oled.h"
#include "spi.h"
#include "pld.h"
#include "net_http.h"
#include "W7500x_wztoe.h"
#include "web_01.h"
#include "web_info.h"
#include "httpParser.h"
#include "httpServer.h"
#include "libc.h"

void api_init(void);
static void net_init(void);

u8 cgi_page(void *req, char *buf, u32 *len, u32 *type);
u8 cgi_info(void *req, char *buf, u32 *len, u32 *type);
u8 cgi_pld (void *req, char *buf, u32 *len, u32 *type);
u8 cgi_spi (void *req, char *buf, u32 *len, u32 *type);

int cgi_ng_page(http_socket *socket);

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

char buffer_tx[2048];
char buffer_rx[2048];
char buffer_dhcp[2048];

int main(void)
{
  dhcp dhcp_session;
  int  dhcp_state;
  http_server http;
  http_socket httpsock;
  http_content httpcontent[2];
  u8  socknumlist[4] = {4, 5, 6, 7};
  char oled_msg[17];
  char *pnt;

  api_init();
  uart_puts(" * eCowLogic firmware \r\n");

  dhcp_session.socket = 2;
  dhcp_session.buffer = (u8 *)buffer_dhcp;
  dhcp_init(&dhcp_session);
  
  oled_pos(1, 0);
  oled_puts("Reseau (DHCP)   ");
  while(1)
  {
    dhcp_state = dhcp_run(&dhcp_session);
    if (dhcp_state == DHCP_IP_LEASED)
      break;
  }  
  pnt = oled_msg;
  pnt += b2ds(pnt, dhcp_session.dhcp_my_ip[0]); *pnt++ = '.';
  pnt += b2ds(pnt, dhcp_session.dhcp_my_ip[1]); *pnt++ = '.';
  pnt += b2ds(pnt, dhcp_session.dhcp_my_ip[2]); *pnt++ = '.';
  pnt += b2ds(pnt, dhcp_session.dhcp_my_ip[3]);
  for ( ; pnt < (oled_msg + 16); pnt++)
    *pnt = ' ';
  oled_msg[16] = 0;
  uart_puts("DHCP: LEASED ! ");
  uart_puts(oled_msg); uart_puts("\r\n");
  oled_pos(1, 0);
  oled_puts(oled_msg);
  
  spi_init();
  pld_init();
  net_init();
  
  /* NOTE: The two HTTP stacks are maintened during migration */
  
  /* Init HTTP content */
  strcpy(httpcontent[0].name, "/p");
  httpcontent[0].wildcard = 1;
  httpcontent[0].cgi = cgi_ng_page;
  httpcontent[0].next = &httpcontent[1];
  /* Init HTTP content */
  strcpy(httpcontent[1].name, "/test");
  httpcontent[1].wildcard = 0;
  httpcontent[1].cgi = cgi_ng_page;
  httpcontent[1].next = 0;
  /* Init the new HTTP layer */
  http.port = 808;
  httpsock.id = 3;
  httpsock.state = 0;
  httpsock.server = &http;
  http.socks = &httpsock;
  http.contents = &httpcontent[0];
  http_init(&http);
  
  /* HTTP Server Initialization  */
  httpServer_init((u8 *)buffer_tx, (u8 *)buffer_rx, 4, socknumlist);
  
  reg_httpServer_webCgi((u8 *)"index.html", (u32)cgi_page);
  reg_httpServer_webCgi((u8 *)"/p/",  (u32)cgi_page);
  reg_httpServer_webCgi((u8 *)"/info",(u32)cgi_info);
  reg_httpServer_webCgi((u8 *)"/pld", (u32)cgi_pld);
  reg_httpServer_webCgi((u8 *)"/spi", (u32)cgi_spi);
  display_reg_webContent_list();
  
  while(1)
  {
    int i;
    
    /* HTTP Server handler */
    for(i = 0; i < 4; i++)
      httpServer_run(i);
    
    http_run(&http);
  }
}

const u8 static_ip[4]   = { 192, 168,   1, 15 };
const u8 static_mask[4] = { 255, 255, 255,  0 };
const u8 static_gw[4]   = { 192, 168,   1,  1 };

static void net_init(void)
{
  setSIPR(static_ip);
  setSUBR(static_mask);
  setGAR (static_gw);
}

int cgi_ng_page(http_socket *socket)
{
  uart_puts("cgi_ng_page()\r\n");
  return(0);
}

u8 cgi_page(void *req, char *buf, u32 *len, u32 *type)
{
  st_http_request *request = (st_http_request *)req;
  
  uart_puts("main::cgi_page() uri=");
  uart_puts((char *)request->URI);
  uart_puts("\r\n");
  
  if (strncmp((char *)request->URI, "/p/", 3) == 0)
  {
    char *pnt = (char *)request->URI;
    fs_entry entry;
    int found = 0;
    int l;
    int i;
    
    pnt += 3;
    if (strlen(pnt) > 8)
      pnt[8] = 0;
    uart_puts("Request FILE ");
    uart_puts(pnt);
    uart_puts("\r\n");
    for (i = 0; i < 128; i++)
    {
      if ( ! fs_getentry(i, &entry))
        break;
      if (strcmp(pnt, entry.name) == 0)
      {
        found = 1;
        break;
      }
    }
    if (found)
    {
      u32 offset;
      
      uart_puts("Found ! "); uart_puthex8(i); uart_puts("\r\n");
      
      /* If this is the first packet, data length is the file size */
      if (request->response_len  == 0)
        request->response_len = entry.size;
      
      i = request->response_len;
      if (i > 1024)
        i = 1024;
      offset  = entry.start;
      offset += (entry.size - request->response_len);
      uart_puts("read at "); uart_puthex(offset); uart_puts("\r\n");
      flash_read(offset, (u8 *)buf, i);
      *len = i;
      
      l = strlen(entry.name);
      if (l >= 4)
      {
        pnt = entry.name;
        pnt += (l - 4);
        uart_puts("extension ");
        uart_puts(pnt);
        uart_puts("\r\n");
        if (strcmp(pnt, ".png") == 0)
          *type = 2;
        if (strcmp(pnt, ".css") == 0)
          *type = 3;
        if (strcmp(pnt, ".jpg") == 0)
          *type = 4;
      }
      uart_puts("File size "); uart_puthex16(entry.size);
      request->response_len -= i;
      uart_puts(" remains "); uart_puthex16(request->response_len);
      uart_puts("\r\n");
    }
    else
      /* Not found :( */
      return(0);
  }
  else
  {
    strcpy(buf, web_01);
    *len = strlen((char *)web_01);
  }

  return 1;
}

u8 cgi_info(void *req, char *buf, u32 *len, u32 *type)
{
  st_http_request *request = (st_http_request *)req;
  
  uart_puts("main::cgi_info() uri=");
  uart_puts((char *)request->URI);
  uart_puts("\r\n");
  
  strcpy(buf, web_info);
  *len = strlen((char *)web_info);
  *type = 1;
  
  return 1;
}

u8 cgi_pld(void *req, char *buf, u32 *result_len, u32 *type)
{
  u32  content_length;
  char str[8];
  char *pnt;
  char *file;
  int mph_len;
  int len;
  int i;
  
  st_http_request *request = (st_http_request *)req;
  
  if (request->priv == 0)
  {
    pld_init();
    for (i = 0; i < 7500; i++)
      ;
    pld_load_start();
    
    content_length = 0;
  
    pnt = (char *)request->header;
    while(pnt)
    {
      if (strncmp(pnt, "Content-Length:", 15) == 0)
      {
        pnt += 16;
        for (i = 0; i < 7; i++)
        {
          if ( (*pnt < '0') || (*pnt > '9') )
            break;
          str[i] = *pnt;
          pnt++;
        }
        str[i] = 0;
        content_length = atoi(str);
        break;
      }
      pnt = strchr(pnt, 0x0A);
      if (pnt)
        pnt++;
    }
  
    pnt = (char *)request->body;
    while(pnt != 0)
    {
      if ( (pnt[0] == 0x0d) && (pnt[1] == 0x0a) &&
           (pnt[2] == 0x0d) && (pnt[3] == 0x0a) )
      {
    	/* Get a pointer on datas (after multipart header) */
    	file = (pnt + 4);
    	/* Cut the string between header and body */
    	pnt[0] = 0;
    	break;
      }
      /* Search the next CR */
      pnt = strchr(pnt + 1, 0x0d);
    }
    /* Multipart header length */
    mph_len = ((u32)file - (u32)request->body);
    /* Received length */
    len = (u32)request->buffer + request->length - (u32)file;
    
    pld_load((const u8 *)file, len);
    
    if (len < (content_length - mph_len))
      request->status = 1;
  
    request->priv = (void *)(content_length - mph_len - len);
  }
  else
  {
    len = (int)request->priv;
    
    pld_load(request->buffer, request->length);
    
    len -= request->length;
    request->priv = (void *)len;
    
    if (len <= 0)
    {
      pld_load_end();
      request->status = 0;
      
      /* Make HTTP result */
      strcpy(buf, "PLD loaded !");
      *result_len = 12;
    }
  }
  
  return 1;
}

u8 cgi_spi(void *req, char *buf, u32 *len, u32 *type)
{
  u8 rd;
  u8 wr;
  int i;
  u8 *pnt;
  
  st_http_request *request = (st_http_request *)req;
  
  uart_puts("main::cgi_spi() uri=");
  uart_puts((char *)request->URI);
  uart_puts("  ");
  
  pnt = (u8 *)request->body;
  pnt = (u8 *)strchr((char *)pnt, '=');
  if (pnt)
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
  else
    wr = 0x5A;
  
  pld_cs(1);
  spi_wr(wr);
  rd = spi_rd();
  pld_cs(0);
  
  uart_puthex8(rd); uart_puts("\r\n");
  
  i = b2ds(buf, rd);
  buf[i] = 0;
  *len = i;

  return 1;
}

int b2ds(char *d, int n)
{
  int count = 0;
  
  if (n > 9999)
  {
    *d = (n / 10000) + '0';
    n -= ((n / 10000) * 10000);
    d++;
    count++;
  }
  if ( (n > 999) || count)
  {
    *d = (n / 1000) + '0';  
    n -= ((n / 1000) * 1000);
    d++; 
    count++;
  }
  if ((n > 99) || count)
  {
    *d = (n / 100) + '0';
    n -= ((n / 100) * 100);
    d++;
    count++;
  }
  if ( (n > 9) || count)
  {
    *d = (n / 10) + '0';
    n -= ((n / 10) * 10);
    d++;
    count ++;
  }
  *d = n + '0';
  count ++;

  return(count);
}

void delay(__IO uint32_t milliseconds)
{
  TimingDelay = milliseconds;

  while(TimingDelay != 0);
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
/* EOF */
