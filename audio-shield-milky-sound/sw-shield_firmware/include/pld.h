/**
 * eCow-logic - Embedded probe main firmware
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
#ifndef PLD_H
#define PLD_h

#include "types.h"

void pld_init(void);
void pld_load_start(void);
void pld_load(const unsigned char *bitstream, u32 len);
void pld_load_end(void);
void pld_cs(int en);

#endif
