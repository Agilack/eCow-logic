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
#include "flash.h"
#include "flash_fs.h"
#include "types.h"

int fs_getentry(int n, fs_entry *entry)
{
  u8   buffer[16];
  u32  val;
  int  offset;
  int  i;
  
  /* If no entry specified ... nothing to do */
  if (entry == 0)
    return(0);
  /* Only 128 entries are supported */
  if (n > 128)
    return(0);
  
  /* Compute entry offset */
  offset = (n << 4);
  /* Read entry from flash */
  flash_read(offset, buffer, 16);
  
  /* If entry is empty */
  if (buffer[0] == 0xFF)
    return(0);
  
  /* Copy file name */
  for (i = 0; i < 8; i++)
    entry->name[i] = buffer[i];
  entry->name[8] = 0;
  /* Copy file start offset */
  val  = (buffer[11] << 24);
  val |= (buffer[10] << 16);
  val |= (buffer[9]  <<  8);
  val |= (buffer[8]       );
  entry->start = val;
  /* Copy file size */
  val  = (buffer[15] << 24);
  val |= (buffer[14] << 16);
  val |= (buffer[13] <<  8);
  val |= (buffer[12]      );
  entry->size  = val;
  
  return(1);
}
/* EOF */
