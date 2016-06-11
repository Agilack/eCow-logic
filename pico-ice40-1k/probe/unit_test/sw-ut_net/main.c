/**
 * eCow-logic - Network (UDP) unit-test
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
#include "W7500x.h"
#include "miim.h"
#include "W7500x_wztoe.h"

void delay(__IO uint32_t milliseconds);
void net_init(void);

static __IO uint32_t TimingDelay;

u8 net_buffer[16];

int main(void)
{
  uint8_t mac_addr[6] = {0x00, 0x08, 0xDC, 0x71, 0x72, 0x77};
  uint8_t src_addr[4] = {192, 168,   1, 100};
  uint8_t gw_addr[4]  = {192, 168,   1,   1};
  uint8_t sub_addr[4] = {255, 255, 255,   0};
  uint8_t dst_addr[4];
  uint16_t dst_port;
  u32 len;
  int i;
  
  hw_init();
  uart_init();
  
  /* SysTick_Config */
  SysTick_Config( hw_getfreq() / 1000 );

  uart_puts("Network UDP Unit Test\r\n");
  
  /* Set WZ_100US Register */
  setTIC100US( hw_getfreq() / 10000 );

  /* PHY Link Check via gpio mdio */
  while( link() == 0x0 )
  {  
    uart_putc('.');
    delay(500);
  }  
  uart_puts("PHY is linked. \r\n");

  /* Network Configuration (Default setting) */
  setSHAR(mac_addr);  
  setSIPR(src_addr);
  setGAR (gw_addr);
  setSUBR(sub_addr);

  net_init();

  while(1)
  {
    len = getSn_RX_RSR(7);
    if (len == 0)
      continue;
    
    /* Read received datas */
    uart_puts("Datas ... ");
    if (len > 16)
      len = 16;
    wiz_recv_data(7, net_buffer, len);
    uart_puthex(len);
    uart_puts("\r\n");
    /* Dump the received data in hex */
    for (i = 0; i < len; i++)
    {
      uart_puthex8(net_buffer[i]);
      uart_putc(' ');
    }
    uart_puts("\r\n");
    /* Mark received datas as read */
    setSn_CR(7, Sn_CR_RECV);
    while(getSn_CR(7))
      ;
    
    /* Send result : copy sender address as destination */
    dst_addr[0] = net_buffer[0]; dst_addr[1] = net_buffer[1];
    dst_addr[2] = net_buffer[2]; dst_addr[3] = net_buffer[3];
    setSn_DIPR (7, dst_addr);
    /* Send result : copy sender port as destination */
    dst_port = (net_buffer[4] << 8) | net_buffer[5];
    setSn_DPORT(7, dst_port);
    /* Send datas */
    wiz_send_data(7, (u8 *)"Hello World!\r\n", 14);
    setSn_CR(7, Sn_CR_SEND);
    while( getSn_CR(7) )
      ;
  }
}

void net_init(void)
{
  /* Set socket 7 as UDP */
  setSn_MR  (7, Sn_MR_UDP);
  /* Configure socket to use port 1234 */
  setSn_PORT(7, 1234);
  /* Send "Open" command to the socket */
  setSn_CR  (7,Sn_CR_OPEN);
  /* Wait until TOE accept the "open" command */
  while( getSn_CR(7) )
    ;
  /* Wait until socket initialisation complete */
  while(getSn_SR(7) == SOCK_CLOSED)
    ;
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
