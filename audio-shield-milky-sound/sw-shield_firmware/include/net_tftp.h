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
#ifndef NET_TFTP_H
#define NET_TFTP_H
#include "types.h"

typedef struct _tftp_session
{
  int state;
  u8  server[4];
  u32 timestamp;
  u32 lastblock;
  u32 length;
  const char *filename;
  u8  *data;
} tftp;

extern void (*tftp_init)(tftp *session);
extern int  (*tftp_run) (tftp *session);
extern void (*tftp_ack) (tftp *session);
extern void (*tftp_stop)(tftp *session);

#endif
