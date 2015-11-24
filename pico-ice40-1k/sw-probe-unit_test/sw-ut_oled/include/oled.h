/**
 * eCow-logic - OLED unit-test
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
#ifndef OLED_H
#define OLED_h

#define MODE_CMD  0
#define MODE_DATA 1

void oled_init(void);
void oled_wr  (u8 c);
void oled_cs  (int en);
void oled_dc  (int dc);

#endif
