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
#include "net_tftp.h"
#include "oled.h"
#include "spi.h"
#include "pld.h"
#include "W7500x_wztoe.h"

void api_init(void);
static void net_init(void);

void delay(__IO uint32_t milliseconds);

static __IO uint32_t TimingDelay;

const char pld_file[] = "ecow.bin";

int main(void)
{
  tftp tftp_session;
  u32  tftp_block;
  int flag = 0;
  
  api_init();
  uart_puts(" * eCowLogic TFTP firmware \r\n");
  
  spi_init();
  pld_init();
  net_init();
  
  tftp_init(&tftp_session);
  tftp_session.filename = pld_file;
  tftp_session.server[0] = 192;
  tftp_session.server[1] = 168;
  tftp_session.server[2] = 1;
  tftp_session.server[3] = 10; 
  
  uart_puts(" * tftp init complete\r\n");
  
  while(1)
  {
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
