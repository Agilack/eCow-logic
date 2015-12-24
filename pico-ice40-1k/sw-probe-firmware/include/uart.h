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
#ifndef UART_H
#define UART_H

#include "types.h"

void uart_init(void);
extern void (*uart_putc)    (u8 c);
extern void (*uart_puts)    (char *s);
extern void (*uart_puthex8) (const u8 c);

void uart_puthex  (u32 c);
void uart_puthex16(const u16 c);

#endif
