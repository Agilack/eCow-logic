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
    .syntax unified
    .arch armv6-m

    .section .api_entry
    .align 4
api_entry:

api_dhcp:
    .long    DHCP_init
    .long    DHCP_run
    .long    0x00000000 /* Rfu */
    .long    DHCP_time_handler

api_tftp:
    .long    tftp_init
    .long    tftp_run
    .long    tftp_ack
    .long    tftp_stop

    .align 4
api_oled:
    .long    0x00000000 /* ToDo: oled_config */
    .long    oled_pos
    .long    oled_puts
    .long    0x00000000 /* ToDo: oled_wr */

    .align 4
api_uart:
    .long    uart_putc
    .long    uart_puts
    .long    uart_puthex8
    .long    0x00000000 /* ToDo: uart_getc */

    .align 4
api_flash:
    .long    flash_read
    .long    flash_write
    .long    flash_erase
    .long    fs_getentry

    .align 4
api_config:
    .long    cfg_get
    .long    cfg_set
    .long    0x00000000
    .long    cfg_clear

    .align 4
api_http:
    .long    http_init
    .long    http_run
    .long    http_get_header
    .long    http_send_header
