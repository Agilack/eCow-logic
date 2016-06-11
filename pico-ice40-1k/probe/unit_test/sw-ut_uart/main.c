/**
 * eCow-logic - UART unit-test
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
#include "system_W7500x.h"
#include "W7500x_gpio.h"
#include "W7500x_uart.h"

void hw_init_led(void);
void delay_ms(__IO uint32_t nCount);

GPIO_InitTypeDef GPIO_InitDef;
UART_InitTypeDef UART_InitStructure;

int main(void)
{
  SystemInit();
  hw_init_led();
  
  /* Configure UART2 */
  S_UART_Init(115200);
  
  while(1)
  {
    uint8_t c;
    
    /* Led Blue On */
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);

    /* Send one byte to UART2  */
    S_UART_SendData('H');
    S_UART_SendData('e');
    S_UART_SendData('l');
    S_UART_SendData('l');
    S_UART_SendData('o');
    S_UART_SendData('\r');
    S_UART_SendData('\n');
    
    /* Led Blue Off */
    GPIO_SetBits  (GPIOC, GPIO_Pin_5);
    
    delay_ms(250);

//    c = S_UartGetc();
//    S_UartPutc(c);
//    continue;
    if (S_UART_GetFlagStatus(S_UART_STATE_RX_BUF_FULL) == SET)
    {
      S_UART_SendData( S_UART_ReceiveData() );
    }
  }
}

void hw_init_led(void)
{
  /* GPIO LED(R) Set */
  GPIO_InitDef.GPIO_Pin = GPIO_Pin_8; // Set to Pin_5 (LED(R))
  GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
  GPIO_Init(GPIOC, &GPIO_InitDef);
  PAD_AFConfig(PAD_PC,GPIO_Pin_8, PAD_AF1); // PAD Config - LED used 2nd Function

  GPIO_SetBits  (GPIOC, GPIO_Pin_8);

  /* GPIO LED(G) Set */
  GPIO_InitDef.GPIO_Pin = GPIO_Pin_9; // Set to Pin_5 (LED(G))
  GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
  GPIO_Init(GPIOC, &GPIO_InitDef);
  PAD_AFConfig(PAD_PC,GPIO_Pin_9, PAD_AF1);	// PAD Config - LED used 2nd Function

  GPIO_SetBits  (GPIOC, GPIO_Pin_9);
  
  /* GPIO LED(B) Set */
  GPIO_InitDef.GPIO_Pin = GPIO_Pin_5; // Set to Pin_5 (LED(B))
  GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
  GPIO_Init(GPIOC, &GPIO_InitDef);
  PAD_AFConfig(PAD_PC,GPIO_Pin_5, PAD_AF1); // PAD Config - LED used 2nd Function

  GPIO_SetBits  (GPIOC, GPIO_Pin_5);
}  

void delay_ms(__IO uint32_t nCount)
{
    volatile uint32_t delay = nCount * 2500; // approximate loops per ms at 24 MHz, Debug config
    for(; delay != 0; delay--)
        __NOP();
}
/* EOF */
