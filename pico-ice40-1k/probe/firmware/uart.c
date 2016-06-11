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
/* EOF */
