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
#ifndef SPI_H
#define SPI_H

#include "hardware.h"

#define SPI0_CR0    (SSP0_ADDR + 0x00)
#define SPI0_CR1    (SSP0_ADDR + 0x04)
#define SPI0_DATA   (SSP0_ADDR + 0x08)
#define SPI0_STATUS (SSP0_ADDR + 0x0C)
#define SPI0_CLK    (SSP0_ADDR + 0x10)

void spi0_init(void);
u8   spi_rd   (void);
void spi_wr   (u32 c);
void spi_flush(void);
void spi_wait (void);

#endif
