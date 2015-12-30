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
#include "hardware.h"
#include "uart.h"
#include "net_dhcp.h"
#include "net_tftp.h"
#include "oled.h"
#include "spi.h"
#include "pld.h"
#include "W7500x_wztoe.h"
#include "web_01.h"
#include "httpParser.h"
#include "httpServer.h"
#include "libc.h"

void api_init(void);
static void net_init(void);

u8 cgi_page(void *req, char *buf, u32 *len);
u8 cgi_pld (void *req, char *buf, u32 *len);

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

const char pld_file[] = "ecow.bin";

char buffer_tx[2048];
char buffer_rx[2048];
char buffer_dhcp[2048];

int main(void)
{
  tftp tftp_session;
  u32  tftp_block;
  int  flag = 0;
  dhcp dhcp_session;
  int  dhcp_state;
  u8  socknumlist[4] = {4, 5, 6, 7};

  
  api_init();
  uart_puts(" * eCowLogic TFTP firmware \r\n");

  dhcp_session.socket = 2;
  dhcp_session.buffer = (u8 *)buffer_dhcp;
  dhcp_init(&dhcp_session);
  
  while(1)
  {
    dhcp_state = dhcp_run(&dhcp_session);
    if (dhcp_state == DHCP_IP_LEASED)
      break;
  }  
  uart_puts("DHCP: LEASED ! ");
  uart_puthex8(dhcp_session.dhcp_my_ip[0]); uart_putc(' ');
  uart_puthex8(dhcp_session.dhcp_my_ip[1]); uart_putc(' ');
  uart_puthex8(dhcp_session.dhcp_my_ip[2]); uart_putc(' ');
  uart_puthex8(dhcp_session.dhcp_my_ip[3]); uart_puts("\r\n");
  
  spi_init();
  pld_init();
  net_init();
  
  /* HTTP Server Initialization  */
  httpServer_init((u8 *)buffer_tx, (u8 *)buffer_rx, 4, socknumlist);
  
/*  tftp_init(&tftp_session);
  tftp_session.filename = pld_file;
  tftp_session.server[0] = 192;
  tftp_session.server[1] = 168;
  tftp_session.server[2] = 1;
  tftp_session.server[3] = 10; 
  
  uart_puts(" * tftp init complete\r\n");
  */
  
  reg_httpServer_webCgi((u8 *)"index.html", (u32)cgi_page);
  reg_httpServer_webCgi((u8 *)"/p",  (u32)cgi_page);
  reg_httpServer_webCgi((u8 *)"/pld",(u32)cgi_pld);
  display_reg_webContent_list();
  
  while(1)
  {
    int i;
    
    /* HTTP Server handler */
    for(i = 0; i < 4; i++)
      httpServer_run(i);
    continue;
    
    tftp_run(&tftp_session);
    
    switch( tftp_session.state )
    {
      case 2:
        if (flag == 0)
        {
          pld_load_start();
          uart_puts(" * Load PLD ");
          flag++;
          oled_line(0);
          oled_puts("Chargement      ");
        }
        
      case 3:
        if (tftp_session.lastblock == tftp_block)
          break;
        if (tftp_session.length > 12)
        {
          int len;
          u8  *pnt;

          uart_putc('.');
          
          len = tftp_session.length - 12;
          pnt = tftp_session.buffer; pnt += 12;
          pld_load(pnt, len);
        }
        tftp_block = tftp_session.lastblock;
        if (tftp_session.state == 3)
        {
          int i;
          tftp_session.state = 90;
          uart_puts("\r\n");
          pld_load_end();
          oled_line(0);
          oled_puts("Lancement       ");
          tftp_session.state = 98;
          for(i = 0; i < 0x1000; i++)
          {
            if ( *(volatile u32 *)MM_GPIOC & 0x00000010)
            {
              tftp_session.state = 90;
              break;
            }
          }
        }
        break;
        
      case 90:
        uart_puts(" * FPGA started :)\r\n");
        oled_line(0);
        oled_puts("FPGA ok         ");
        tftp_session.state = 126;
        break;
        
      case 98:
        uart_puts(" * FPGA not started :(\r\n");
        oled_line(0);
        oled_puts("Erreur FPGA");
        tftp_session.state = 127;
        break;
      
      case 99:
        uart_puts(" * TFTP: error\r\n");
        tftp_session.state = 127;
        oled_line(0);
        oled_puts("Erreur tftp");
        break;
    }
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

u8 cgi_page(void *req, char *buf, u32 *len)
{
  st_http_request *request = (st_http_request *)req;
  
  uart_puts("main::cgi_page() uri=");
  uart_puts((char *)request->URI);
  uart_puts("\r\n");
  
  strcpy(buf, web_01);
  *len = strlen((char *)web_01);

  return 1;
}

u8 cgi_pld(void *req, char *buf, u32 *result_len)
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
    
    if (len < (content_length - mph_len))
      request->status = 1;
  
    request->priv = (void *)(content_length - mph_len - len);
  }
  else
  {
    len = (int)request->priv;
    
    len -= request->length;
    request->priv = (void *)len;
    
    if (len <= 0)
    {
      request->status = 0;
      
      /* Make HTTP result */
      strcpy(buf, "PLD loaded !");
      *result_len = 12;
    }
  }
  
  return 1;
}

int b2ds(char *d, int n)
{
  int count = 0;
  
  if (n > 999)
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
