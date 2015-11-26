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
#ifndef TYPES_H
#define TYPES_H

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;

/* Standard stdint definitions */
typedef signed char             int8_t;
typedef signed int              int32_t;
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;

typedef int		ptrdiff_t;

#define NULL 0
#define __IO volatile

#endif
