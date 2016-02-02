/**
 * eCow-logic - Bootloader
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
#ifndef FLASH_H
#define FLASH_H

#include "types.h"

int  flash_init (u32 *id);
int  flash_status(void);
void flash_read (u32 addr, u8 *buffer, u32 len);
void flash_write(u32 addr, u8 *buffer, u32 len);
void flash_erase(u32 addr);
void flash_cs   (int en);

#endif
