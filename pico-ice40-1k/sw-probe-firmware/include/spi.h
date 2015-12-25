/**
 * eCow-logic - Embedded probe main firmware
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
#ifndef SPI_H
#define SPI_H

#include "hardware.h"

#define SPI1_CR0    (SSP1_BASE + 0x00)
#define SPI1_CR1    (SSP1_BASE + 0x04)
#define SPI1_DATA   (SSP1_BASE + 0x08)
#define SPI1_STATUS (SSP1_BASE + 0x0C)
#define SPI1_CLK    (SSP1_BASE + 0x10)

void spi_init(void);
void spi_wr  (u32 c);
void spi_wait(void);

#endif
