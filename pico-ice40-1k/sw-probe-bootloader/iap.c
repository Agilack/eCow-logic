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
#include "hardware.h"
#include "iap.h"
#include "uart.h"

#define IAP_ENTRY 0x1FFF1001

void iap_erase(u32 addr)
{
  void (*fpnt)(u32, u32, u8*, u32);
  
  fpnt = (void (*)(u32, u32, u8*, u32))0x1FFF1001;

  __asm volatile ("cpsid i" : : : "memory");
  
  fpnt(19, addr, 0, 0);
  
  __asm volatile ("cpsie i" : : : "memory");
}

void iap_write(u32 addr, u8 *s, int len)
{
  void (*fpnt)(u32, u32, u8*, u32);
  
  fpnt = (void (*)(u32, u32, u8*, u32))0x1FFF1001;
  
  /* Disable interrupts before calling IAP */
  __asm volatile ("cpsid i" : : : "memory");
  /* Call IAP */
  fpnt(0x22, addr, s, len);
  /* Enable interrupts */
  __asm volatile ("cpsie i" : : : "memory");
}
/* EOF */
