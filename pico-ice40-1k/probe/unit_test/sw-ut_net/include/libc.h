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
#ifndef LIBC_H
#define LIBC_H

#include "types.h"

void *memcpy(void *dst, const void *src, int n);
void *memset(void *dst, int value, int n);
char *strcpy(char *dest, const char *src);
int strlen(char *str);
#endif
