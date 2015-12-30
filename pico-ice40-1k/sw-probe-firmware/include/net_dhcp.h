/**
 * eCow-logic - Bootloader
 *
 * Copyright (c) 2015 Saint-Genest Gwenael <gwen@agilack.fr>
 *
 * Original file from Wiznet ioLibrary:
 * Authors: Eric Jung & MidnightCow
 * Copyright (c) 2013, WIZnet Co., LTD.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef _DHCP_H_
#define _DHCP_H_

#include "types.h"

typedef struct _dhcp_session
{
  int socket;
  int state;
  u32 lease_time;
  int retry;
  int tick_1s;
  int tick_next;
  u32 dhcp_xid;
  /* DHCP Received Configuration */
  u8  dhcp_my_ip[4];  /* IP address from DHCP      */
  u8  dhcp_my_gw[4];  /* Gateway address from DHCP */
  u8  dhcp_my_sn[4];  /* Subnet mask from DHCP     */
  u8  dhcp_my_dns[4]; /* DNS address from DHCP     */
  u8  dhcp_sip[4];    /* DHCP server ip addres     */
  u8  dhcp_siaddr[4]; /* Bootstrap server address  */
  /* Temporary / internal buffers */
  u8  old_ip[4];
  u8  dhcp_chaddr[6]; /* DHCP Client MAC address.  */
  u8 *buffer;
} dhcp;

/* 
 * @brief return value of @ref DHCP_run()
 */
enum
{
   DHCP_FAILED = 0,  ///< Procssing Fail
   DHCP_RUNNING,     ///< Procssing DHCP proctocol
   DHCP_IP_ASSIGN,   ///< First Occupy IP from DHPC server      (if cbfunc == null, act as default default_ip_assign)
   DHCP_IP_CHANGED,  ///< Change IP address by new ip from DHCP (if cbfunc == null, act as default default_ip_update)
   DHCP_IP_LEASED,   ///< Stand by 
   DHCP_STOPPED      ///< Stop procssing DHCP protocol
};

/*
 * @brief DHCP client initialization
 * @param session - Structure that describe the current DHCP config
 */
extern void (*dhcp_init)(dhcp *session);

/*
 * @brief DHCP client in the main loop
 * @param session - Structure that describe the current DHCP config
 * @return    The current DHCP client status
 */ 
extern int (*dhcp_run)(dhcp *session);

#endif	/* _DHCP_H_ */
