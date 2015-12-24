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
#include "uart.h"
#include "types.h"

void uart_puthex (u32 c)
{
    uart_puthex8((c >> 24) & 0xFF);
    uart_puthex8((c >> 16) & 0xFF);
    uart_puthex8((c >>  8) & 0xFF);
    uart_puthex8( c        & 0xFF);
}
void uart_puthex16(const u16 c)
{
	uart_puthex8(c >> 8);
	uart_puthex8(c);
}
void uart_puthex4(const u8 c)
{
	if (c == 0)
	  uart_putc('0');
        else if (c == 1)
          uart_putc('1');
        else if (c == 2)
          uart_putc('2');
        else if (c == 3)
          uart_putc('3');
        else if (c == 4)
          uart_putc('4');
        else if (c == 5)
          uart_putc('5');
        else if (c == 6)
          uart_putc('6');
        else if (c == 7)
          uart_putc('7');
        else if (c == 8)
          uart_putc('8');
        else if (c == 9)
          uart_putc('9');
        else if (c == 10)
          uart_putc('A');
        else if (c == 11)
          uart_putc('B');
        else if (c == 12)
          uart_putc('C');
        else if (c == 13)
          uart_putc('D');
        else if (c == 14)
          uart_putc('E');
        else if (c == 15)
          uart_putc('F');
}
/* EOF */
