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
#include "W7500x_wztoe.h"

#undef _DHCP_DEBUG_

/* Retry to processing DHCP */
#define	MAX_DHCP_RETRY          2        ///< Maxium retry count
#define	DHCP_WAIT_TIME          10       ///< Wait Time 10s

#define SOCK_ERROR            0
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)    ///< Timeout occurred

/* UDP port numbers for DHCP */
#define DHCP_SERVER_PORT      	67	      ///< DHCP server port number
#define DHCP_CLIENT_PORT         68	      ///< DHCP client port number

#define MAGIC_COOKIE             0x63825363  ///< Any number. You can be modifyed it any number

#define DHCP_HOST_NAME           "eCowLogic\0"

typedef struct _dhcp_session
{
  int socket;
  int state;
  u32 lease_time;
  int retry;
  unsigned int tick_1s;
  unsigned int tick_next;
  u32 dhcp_xid;
  /* DHCP Received Configuration */
  u8  dhcp_my_ip[4];  /* IP address from DHCP      */
  u8  dhcp_my_gw[4];  /* Gateway address from DHCP */
  u8  dhcp_my_sn[4];  /* Subnet mask from DHCP     */
  u8  dhcp_my_dns[4]; /* DNS address from DHCP     */
  u8  dhcp_sip[4];    /* DHCP server ip addres     */
  u8  dhcp_siaddr[4]; /* Bootstrap server address  */
  u8 *dhcp_file;
  /* Temporary / internal buffers */
  u8  old_ip[4];
  u8  dhcp_chaddr[6]; /* DHCP Client MAC address.  */
  u8 *buffer;
} dhcp_session;


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
void DHCP_init(dhcp_session *session);

/*
 * @brief DHCP 1s Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler 
 */
void DHCP_time_handler(dhcp_session *session);

/* 
 * @brief Register call back function 
 * @param ip_assign   - callback func when IP is assigned from DHCP server first
 * @param ip_update   - callback func when IP is changed
 * @prarm ip_conflict - callback func when the assigned IP is conflict with others.
 */
void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void));

/*
 * @brief DHCP client in the main loop
 * @return    The value is as the follow \n
 *            @ref DHCP_FAILED     \n
 *            @ref DHCP_RUNNING    \n
 *            @ref DHCP_IP_ASSIGN  \n
 *            @ref DHCP_IP_CHANGED \n
 * 			  @ref DHCP_IP_LEASED  \n
 *            @ref DHCP_STOPPED    \n
 *
 * @note This function is always called by you main task.
 */ 
uint8_t DHCP_run(dhcp_session *session);

/*
 * @brief Stop DHCP procssing
 * @note If you want to restart. call DHCP_init() and DHCP_run()
 */ 
void    DHCP_stop(dhcp_session *session);

#endif	/* _DHCP_H_ */
