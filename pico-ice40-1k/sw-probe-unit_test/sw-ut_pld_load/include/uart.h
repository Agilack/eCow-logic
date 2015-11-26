/**
 * eCow-logic - PLD loader unit-test
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
#ifndef UART_H
#define UART_H

#include "hardware.h"

#define UART2_DATA     (UART2_BASE + 0x00)
#define UART2_STATE    (UART2_BASE + 0x04)
#define UART2_CTRL     (UART2_BASE + 0x08)
#define UART2_ISR      (UART2_BASE + 0x0C)
#define UART2_BAUD     (UART2_BASE + 0x10)

void uart_init(void);
void uart_putc(u8  c);
void uart_puts(char *s);

#endif
