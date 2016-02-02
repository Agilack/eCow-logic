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
#include "dhcp.h"
#include "flash.h"
#include "libc.h"
#include "net_tftp.h"
#include "oled.h"
#include "uart.h"

static void upd_flash   (tftp *session);
int b2ds(char *d, int n);

const static char upd_mem[9] = "ecow.mem";

void tftp_update(dhcp_session *dhcp)
{
  tftp tftp_session;
  
  tftp_session.port = TFTP_PORT_DEFAULT;
  tftp_session.server[0] = dhcp->dhcp_siaddr[0];
  tftp_session.server[1] = dhcp->dhcp_siaddr[1];
  tftp_session.server[2] = dhcp->dhcp_siaddr[2];
  tftp_session.server[3] = dhcp->dhcp_siaddr[3];
  
  upd_flash(&tftp_session);
}

static void upd_flash(tftp *tftp)
{
  char str[17];
  int tftp_block;
  u32 waddr;
  u32 sector, last_sector;
  int len;
  
  tftp_init(tftp);
  tftp->filename = upd_mem;
  
  waddr = 0;
  last_sector= 0xFFFFFFFF;
  
  while(1)
  {
    tftp_run(tftp);
    if ( (tftp->state == 2) ||
         (tftp->state == 3))
    {
        if (tftp->lastblock != tftp_block)
        {
          tftp_block = tftp->lastblock;
          
          strcpy(str, "Load flash      ");
          b2ds(&str[11], tftp_block);
          oled_line(0);
          oled_puts(str);

          sector = (tftp_block << 9) & 0xFFFF0000;
          if (sector != last_sector)
          {
            flash_erase(sector);
            last_sector = sector;
          }
          uart_puts(str); uart_putc('\r');
          
          len = 0;
          if (tftp->length > 4)
          {
            u8 *dat;
            dat = (u8 *)&tftp->data[4];
            
            len = tftp->length - 4;
            if (len > 256)
            {
              flash_write(waddr, dat, 256);
              len   -= 256; /* Update remaining data length */
              dat   += 256; /* Move the source pointer      */
              waddr += 256; /* Update the dest address      */

              /* Wait for the write transfer ends */
              while(flash_status() & 1)
                ;
            }
            flash_write(waddr, dat, len);
            /* Wait for the write transfer ends */
            while(flash_status() & 1)
              ;
            /* Update write address for next packet */
            waddr += len;
          }
        }
        tftp_ack(tftp);
    }
    if (tftp->state == 3)
    {
      uart_crlf();
      break;
    }
    if (tftp->state == 98)
    {
      uart_puts("TFTP timeout, restart\r\n");
      oled_line(0);
      oled_puts("TFTP: timeout!");
      tftp->timestamp = 0x3000;
      tftp->state = 0;
    }
    if (tftp->state == 99)
    {
      uart_puts("upd_firmware() TFTP error 99\r\n");
      break;
    }
  }
}
/* EOF */
