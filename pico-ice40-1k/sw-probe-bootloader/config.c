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
#include "config.h"
#include "iap.h"
#include "libc.h"
#include "types.h"

int cfg_get(char *key, u8 *value)
{
  u8 *pnt, *end;
  int elen, len;
  int found = 0;
  
  pnt = (u8 *)0x0003FF00;
  end = (u8 *)0x0003FFFF;
  
  while( (*pnt != 0xFF) && (pnt < end) )
  {
    elen = *pnt;
    if (strcmp((char *)(pnt+1), key) == 0)
    {
      found = 1;
      break;
    }
    /* Jump to next entry */
    pnt += elen;
  }
  
  if(found)
  {
    if(value)
    {
      pnt ++;
      len = strlen((char *)pnt);
      pnt += len + 1;
      memcpy(value, pnt, (elen - len - 2));
    }
    return(0);
  }
  return(-1);
}

int cfg_set(char *key, u8 *value, int vlen)
{
  u8  buffer[256];
  u8 *pnt, *end, *pcut;
  int klen, len;
  
  /* Get a copy of config datas */
  memcpy(buffer, (char *)0x0003FF00, 256);
  
  pnt = buffer;
  end = buffer + 256;
  while( (*pnt != 0xFF) && (pnt < end) )
  {
    klen = *pnt;
    if (strcmp((char *)(pnt+1), key))
    {
      /* Jump to next entry */
      pnt += klen;
      continue;
    }
    /* Key already exists, remove it */
    
    pcut = pnt + klen;
    len = (end - pcut);
    memcpy(pnt, pcut, len);
    memset(pcut + len, 0xFF, klen);
  }
  
  klen = strlen(key);
  len  = klen + vlen + 2;
  
  /* Test if there is enough space to save new value */
  if ( (pnt + len) > end)
    return(-1);
  
  *pnt = len; pnt++;
  strcpy((char *)pnt, key);
  pnt += klen + 1;
  memcpy(pnt, value, vlen);
  
  cfg_clear();
  iap_write(0x0003FF00, buffer, 256);
  
  return(0);
}

void cfg_clear(void)
{
  iap_erase(0x0003FF00);
}

#ifdef CFG_DEBUG
void cfg_dump(u32 addr)
{
  int j, k;
  char *v;
  
  v = (char *)addr;
  
  for (j = 0; j < 16; j++)
  {
    for (k = 0; k < 16; k++)
    {
      uart_puthex8(*v);
      uart_putc(' ');
      v++;
    }
    uart_puts("\r\n");
  }
}
#endif
/* EOF */
