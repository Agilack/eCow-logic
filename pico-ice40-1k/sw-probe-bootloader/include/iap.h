/**
 * eCow-logic - Bootloader
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
#ifndef IAP_H
#define IAP_H

#include "types.h"

#define IAP_ENTRY 0x1FFF1001
#define IAP_FCT(id, addr, data, len) \
           ((void(*)(u32,u32,u8*,u32))IAP_ENTRY)(id, addr, data, len);

void iap_erase(u32 addr);
void iap_write(u32 addr, u8 *s, int len);

#endif
