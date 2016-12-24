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
#include "libc.h"

void api_init(void);
static void net_init(void);

int cgi_info(http_socket *socket);
int cgi_spi (http_socket *socket);
int cgi_ng_wav(http_socket *socket);

int cgi_ng_page(http_socket *socket);
int cgi_ng_pld (http_socket *socket);

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

char buffer_dhcp[2048];

int main(void)
{
  dhcp dhcp_session;
  int  dhcp_state;
  http_server http;
  http_socket httpsock[4];
  http_content httpcontent[5];
  char oled_msg[17];
  char *pnt;
  int   step;

  api_init();
  uart_puts(" * eCowLogic firmware \r\n");

  hw_systick( hw_getfreq() / 1000 );

  dhcp_session.socket = 2;
  dhcp_session.buffer = (u8 *)buffer_dhcp;
  dhcp_init(&dhcp_session);
  
  oled_pos(1, 0);
  oled_puts("Reseau (DHCP)   ");
  step = 0;
  while(1)
  {
    dhcp_state = dhcp_run(&dhcp_session);
    if (dhcp_state == DHCP_IP_LEASED)
      break;
    step++;
    oled_pos(1, 13);
    if (step == 1) oled_puts(".  ");
    if (step == 2) oled_puts(".. ");
    if (step == 3) oled_puts("...");
    if (step == 4)
    {
      step = 0;
      dhcp_session.tick_1s++;
      uart_putc('.');
      oled_puts("   ");
    }
    delay(250);
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

  //net_init();
  
  /* Init HTTP content */
  strcpy(httpcontent[0].name, "/pld");
  httpcontent[0].wildcard = 0;
  httpcontent[0].cgi = cgi_ng_pld;
  httpcontent[0].next = &httpcontent[1];
  /* Init HTTP content */
  strcpy(httpcontent[1].name, "/spi");
  httpcontent[1].wildcard = 0;
  httpcontent[1].cgi = cgi_spi;
  httpcontent[1].next = &httpcontent[2];
  /* Init HTTP content */
  strcpy(httpcontent[2].name, "/infos");
  httpcontent[2].wildcard = 0;
  httpcontent[2].cgi = cgi_info;
  httpcontent[2].next = &httpcontent[3];
  /* Init HTTP content */
  strcpy(httpcontent[3].name, "/wav");
  httpcontent[3].wildcard = 0;
  httpcontent[3].cgi = cgi_ng_wav;
  httpcontent[3].next = &httpcontent[4];
  /* Init HTTP content */
  strcpy(httpcontent[4].name, "/");
  httpcontent[4].wildcard = 1;
  httpcontent[4].cgi = cgi_ng_page;
  httpcontent[4].next = 0;
  /* Init HTTP socket */
  httpsock[0].id = 4;
  httpsock[0].state = 0;
  httpsock[0].server = &http;
  httpsock[0].next = &httpsock[1];
  /* Init HTTP socket */
  httpsock[1].id = 5;
  httpsock[1].state = 0;
  httpsock[1].server = &http;
  httpsock[1].next = &httpsock[2];
  /* Init HTTP socket */
  httpsock[2].id = 6;
  httpsock[2].state = 0;
  httpsock[2].server = &http;
  httpsock[2].next = &httpsock[3];
  /* Init HTTP socket */
  httpsock[3].id = 7;
  httpsock[3].state = 0;
  httpsock[3].server = &http;
  httpsock[3].next = 0;
  /* Init the new HTTP layer */
  http.port   = 80;
  http.err404 = 0;
  http.err403 = 0;
  http.keepalive = 10;
  http.socks  = &httpsock[0];
  http.contents = &httpcontent[0];
  http_init(&http);
  
  while(1)
  {
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
  fs_entry entry;
  char  page_name[9];
  char *pnt;
  u32   offset;
  int   found;
  int   type;
  int   l;
  int   i;
  
  if (socket->content_len == 0)
  {
    pnt = (char *)socket->uri;
    /* If the URI start with "/p/" this is a page request */
    if (strncmp(pnt, "/p/", 3) == 0)
    {
      pnt += 3;
      for (i = 0; i < 8; i++)
      {
        if ((pnt[i] == ' ') ||
            (pnt[i] == 0x0D) )
          break;
        page_name[i] = pnt[i];
      }
      page_name[i] = 0;
    }
    /* Else, use the homepage */
    else
      strcpy(page_name, "home.htm");
    
    found = 0;
    for (i = 0; i < 128; i++)
    {
      if ( ! fs_getentry(i, &entry))
        break;
      if (strcmp(page_name, entry.name) == 0)
      {
        found = 1;
        break;
      }
    }
  
    if ( ! found)
    {
      socket->state = HTTP_STATE_NOT_FOUND;
      return(0);
    }
    socket->content_len  = entry.size;
    socket->content_priv = (void *)i;
    
    type = HTTP_CONTENT_PLAIN;
    
    l = strlen(entry.name);
    if (l >= 4)
    {
      pnt = entry.name;
      pnt += (l - 4);
      if (strcmp(pnt, ".htm") == 0)
        type = HTTP_CONTENT_HTML;
      if (strcmp(pnt, ".png") == 0)
        type = HTTP_CONTENT_PNG;
      if (strcmp(pnt, ".css") == 0)
        type = HTTP_CONTENT_CSS;
      if (strcmp(pnt, ".jpg") == 0)
        type = HTTP_CONTENT_CSS;
    }
    
    http_send_header(socket, 200, type);
  }
  else
  {
    i = (int)socket->content_priv;
    fs_getentry(i, &entry);
  }

  i = socket->content_len;
  if (i > 768)
    i = 768;
  offset  = entry.start;
  offset += (entry.size - socket->content_len);
  flash_read(offset, (u8 *)socket->tx, i);
  socket->tx_len += i;

  socket->content_len -= i;
  if (socket->content_len == 0)
    socket->state = HTTP_STATE_SEND;
  else
    socket->state = HTTP_STATE_SEND_MORE;
  
  return(0);
}

int cgi_info(http_socket *socket)
{
  char buffer[256];
  char str[16];
  u8  *dat;
  int  i, l;
  
  uart_puts("main::cgi_info()\r\n");
  
  strcpy(buffer, "{\"model\":\"ecow-logic\",");
  strcat(buffer, "\"version\":\"0.2\",");
  
  dat = (u8 *)0x0003FE00;
  
  strcat(buffer, "\"ident\":[");
  for (i = 0; i < 16; i++)
  {
    l = b2ds(str, dat[i]);
    str[l] = 0;
    strcat(buffer, str);
    if (i != 15)
      strcat(buffer, ", ");
  }
  strcat(buffer, "], ");

  strcat(buffer, "\"fw_version\":[0,0,0,0]");

  strcat(buffer, "}");
  
  socket->content_len = strlen(buffer);
  http_send_header(socket, 200, HTTP_CONTENT_JSON);
  strcpy((char *)socket->tx, buffer);
  socket->tx_len += strlen(buffer);  
  socket->state = HTTP_STATE_SEND;   
  
  return 0;
}

int cgi_ng_wav(http_socket *socket)
{
	u32   content_length;
	char *file;
	int   mph_len;
	char *pnt;
	int   len;
	int   i;
	
	pld_cs(1);
	
	for(i=0;i<1000;i++); // wait
		
	if (socket->content_priv == 0)
	{
		for (i = 0; i < 16; i++) // Get the content length
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
	
	
	pnt = (char *)socket->rx;
	
    while(pnt != 0)
    {
      if ( (pnt[0] == 0x0d) && (pnt[1] == 0x0a) && (pnt[2] == 0x0d) && (pnt[3] == 0x0a) )
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
    
		pld_load((const u8 *)file, len);
	  
    if (len < content_length)
      socket->state = HTTP_STATE_RECV_MORE;
    socket->content_priv = (void *)(content_length - mph_len - len);
  }
  else
	{
		content_length = (u32)socket->content_priv;

		pld_load((u8 *)socket->rx, socket->rx_len);

		if (socket->rx_len < content_length)
		{
			socket->content_priv = (void *)(content_length - socket->rx_len);
		}
		else
		{
			uart_puts("CGI: PLD bitstream loaded !\r\n");
			/* Send more bytes to allow programming process to finish */
			for (i = 0; i < 50; i++)
				spi_wr(0x00);
			
			pld_cs(0);
			
			socket->content_priv = 0;
			http_send_header(socket, 200, 0);
			socket->state = HTTP_STATE_SEND;
		}
	}
	return(0);
}

int cgi_ng_pld(http_socket *socket)
{
  u32   content_length;
  char *file;
  int   mph_len;
  char *pnt;
  int   len;
  int   i;
  
  if (socket->content_priv == 0)
  { 
		uart_puts("CGI: Start loading PLD.\r\n");
	  
		pld_init();
		for (i = 0; i < 7500; i++)
		  ;
		pld_load_start();
		
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
		
		pld_load((const u8 *)file, len);
		
		if (len < content_length)
		  socket->state = HTTP_STATE_RECV_MORE;
		socket->content_priv = (void *)(content_length - mph_len - len);
  }
  else
  {
    content_length = (u32)socket->content_priv;
    
    pld_load((u8 *)socket->rx, socket->rx_len);
    
    if (socket->rx_len < content_length)
    {
      socket->content_priv = (void *)(content_length - socket->rx_len);
    }
    else
    {
      uart_puts("CGI: PLD bitstream loaded !\r\n");
      pld_load_end();
      
      socket->content_priv = 0;
      http_send_header(socket, 200, 0); // pourquoi envoyer entete ?
      socket->state = HTTP_STATE_SEND;
    }
  }
  return(0);
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
  if (TimingDelay)
    TimingDelay--;
}
/* EOF */
