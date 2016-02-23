//*****************************************************************************
//
//! \file dhcp.c
//! \brief DHCP APIs implement file.
//! \details Processig DHCP protocol as DISCOVER, OFFER, REQUEST, ACK, NACK and DECLINE.
//! \version 1.1.0
//! \date 2013/11/18
//! \par  Revision history
//!       <2013/11/18> 1st Release
//!       <2012/12/20> V1.1.0
//!         1. Optimize code
//!         2. Add reg_dhcp_cbfunc()
//!         3. Add DHCP_stop() 
//!         4. Integrate check_DHCP_state() & DHCP_run() to DHCP_run()
//!         5. Don't care system endian
//!         6. Add comments
//!       <2012/12/26> V1.1.1
//!         1. Modify variable declaration: dhcp_tick_1s is declared volatile for code optimization
//! \author Eric Jung & MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include "dhcp.h"
#include "libc.h"
#include "uart.h"

#undef _DHCP_DEBUG_

/* DHCP state machine. */
#define STATE_DHCP_INIT          0        ///< Initialize
#define STATE_DHCP_DISCOVER      1        ///< send DISCOVER and wait OFFER
#define STATE_DHCP_REQUEST       2        ///< send REQEUST and wait ACK or NACK
#define STATE_DHCP_LEASED        3        ///< ReceiveD ACK and IP leased
#define STATE_DHCP_REREQUEST     4        ///< send REQUEST for maintaining leased IP
#define STATE_DHCP_RELEASE       5        ///< No use
#define STATE_DHCP_STOP          6        ///< Stop procssing DHCP

#define DHCP_FLAGSBROADCAST      0x8000   ///< The broadcast value of flags in @ref RIP_MSG 
#define DHCP_FLAGSUNICAST        0x0000   ///< The unicast   value of flags in @ref RIP_MSG

/* DHCP message OP code */
#define DHCP_BOOTREQUEST         1        ///< Request Message used in op of @ref RIP_MSG
#define DHCP_BOOTREPLY           2        ///< Reply Message used i op of @ref RIP_MSG

/* DHCP message type */
#define DHCP_DISCOVER            1        ///< DISCOVER message in OPT of @ref RIP_MSG
#define DHCP_OFFER               2        ///< OFFER message in OPT of @ref RIP_MSG
#define DHCP_REQUEST             3        ///< REQUEST message in OPT of @ref RIP_MSG
#define DHCP_DECLINE             4        ///< DECLINE message in OPT of @ref RIP_MSG
#define DHCP_ACK                 5        ///< ACK message in OPT of @ref RIP_MSG
#define DHCP_NAK                 6        ///< NACK message in OPT of @ref RIP_MSG
#define DHCP_RELEASE             7        ///< RELEASE message in OPT of @ref RIP_MSG. No use
#define DHCP_INFORM              8        ///< INFORM message in OPT of @ref RIP_MSG. No use

#define DHCP_HTYPE10MB           1        ///< Used in type of @ref RIP_MSG
#define DHCP_HTYPE100MB          2        ///< Used in type of @ref RIP_MSG

#define DHCP_HLENETHERNET        6        ///< Used in hlen of @ref RIP_MSG
#define DHCP_HOPS                0        ///< Used in hops of @ref RIP_MSG
#define DHCP_SECS                0        ///< Used in secs of @ref RIP_MSG

#define INFINITE_LEASETIME       0xffffffff	///< Infinite lease time

#define OPT_SIZE                 312               /// Max OPT size of @ref RIP_MSG
#define RIP_MSG_SIZE             (236+OPT_SIZE)    /// Max size of @ref RIP_MSG

/* 
 * @brief DHCP option and value (cf. RFC1533)
 */
enum
{
   padOption               = 0,
   subnetMask              = 1,
   timerOffset             = 2,
   routersOnSubnet         = 3,
   timeServer              = 4,
   nameServer              = 5,
   dns                     = 6,
   logServer               = 7,
   cookieServer            = 8,
   lprServer               = 9,
   impressServer           = 10,
   resourceLocationServer	= 11,
   hostName                = 12,
   bootFileSize            = 13,
   meritDumpFile           = 14,
   domainName              = 15,
   swapServer              = 16,
   rootPath                = 17,
   extentionsPath          = 18,
   IPforwarding            = 19,
   nonLocalSourceRouting   = 20,
   policyFilter            = 21,
   maxDgramReasmSize       = 22,
   defaultIPTTL            = 23,
   pathMTUagingTimeout     = 24,
   pathMTUplateauTable     = 25,
   ifMTU                   = 26,
   allSubnetsLocal         = 27,
   broadcastAddr           = 28,
   performMaskDiscovery    = 29,
   maskSupplier            = 30,
   performRouterDiscovery  = 31,
   routerSolicitationAddr  = 32,
   staticRoute             = 33,
   trailerEncapsulation    = 34,
   arpCacheTimeout         = 35,
   ethernetEncapsulation   = 36,
   tcpDefaultTTL           = 37,
   tcpKeepaliveInterval    = 38,
   tcpKeepaliveGarbage     = 39,
   nisDomainName           = 40,
   nisServers              = 41,
   ntpServers              = 42,
   vendorSpecificInfo      = 43,
   netBIOSnameServer       = 44,
   netBIOSdgramDistServer	= 45,
   netBIOSnodeType         = 46,
   netBIOSscope            = 47,
   xFontServer             = 48,
   xDisplayManager         = 49,
   dhcpRequestedIPaddr     = 50,
   dhcpIPaddrLeaseTime     = 51,
   dhcpOptionOverload      = 52,
   dhcpMessageType         = 53,
   dhcpServerIdentifier    = 54,
   dhcpParamRequest        = 55,
   dhcpMsg                 = 56,
   dhcpMaxMsgSize          = 57,
   dhcpT1value             = 58,
   dhcpT2value             = 59,
   dhcpClassIdentifier     = 60,
   dhcpClientIdentifier    = 61,
   endOption               = 255
};

/*
 * @brief DHCP message format
 */ 
struct _RIP_MSG {
	uint8_t  op;            ///< @ref DHCP_BOOTREQUEST or @ref DHCP_BOOTREPLY
	uint8_t  htype;         ///< @ref DHCP_HTYPE10MB or @ref DHCP_HTYPE100MB
	uint8_t  hlen;          ///< @ref DHCP_HLENETHERNET
	uint8_t  hops;          ///< @ref DHCP_HOPS
	uint32_t xid;           ///< @ref DHCP_XID  This increase one every DHCP transaction.
	uint16_t secs;          ///< @ref DHCP_SECS
	uint16_t flags;         ///< @ref DHCP_FLAGSBROADCAST or @ref DHCP_FLAGSUNICAST
	uint8_t  ciaddr[4];     ///< @ref Request IP to DHCP sever
	uint8_t  yiaddr[4];     ///< @ref Offered IP from DHCP server
	uint8_t  siaddr[4];     ///< No use 
	uint8_t  giaddr[4];     ///< No use
	uint8_t  chaddr[16];    ///< DHCP client 6bytes MAC address. Others is filled to zero
	uint8_t  sname[64];     ///< No use
	uint8_t  file[128];     ///< No use
	uint8_t  OPT[OPT_SIZE]; ///< Option
} __attribute__((packed));

typedef struct _RIP_MSG RIP_MSG;

/* The default callback function */
void default_ip_assign  (dhcp_session *session);
void default_ip_update  (dhcp_session *session);
void default_ip_conflict(dhcp_session *session);

/* send DISCOVER message to DHCP server */
void     send_DHCP_DISCOVER(dhcp_session *session);

/* send REQEUST message to DHCP server */
void     send_DHCP_REQUEST(dhcp_session *session);

/* send DECLINE message to DHCP server */
void     send_DHCP_DECLINE(dhcp_session *session);

/* IP conflict check by sending ARP-request to leased IP and wait ARP-response. */
int8_t   check_DHCP_leasedIP(dhcp_session *session);

/* check the timeout in DHCP process */
uint8_t  check_DHCP_timeout(dhcp_session *session);

/* Intialize to timeout process.  */
void     reset_DHCP_timeout(dhcp_session *session);

/* Parse message as OFFER and ACK and NACK from DHCP server.*/
int8_t   parseDHCPCMSG(void);

/* The default handler of ip assign first */
void default_ip_assign(dhcp_session *session)
{
   setSIPR(session->dhcp_my_ip);
   setSUBR(session->dhcp_my_sn);
   setGAR (session->dhcp_my_gw);
}

/* The default handler of ip chaged */
void default_ip_update(dhcp_session *session)
{
   /* WIZchip Software Reset */
   setMR(MR_RST);
   getMR(); // for delay
   default_ip_assign(session);
   setSHAR(session->dhcp_chaddr);
}

/* The default handler of ip chaged */
void default_ip_conflict(dhcp_session *session)
{
	// WIZchip Software Reset
	setMR(MR_RST);
	getMR(); // for delay
	setSHAR(session->dhcp_chaddr);
}

int32_t dhcp_sendto(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
{
	uint32_t ptr;
	uint8_t  tmp;

	/* Configure remote host address and port */
	setSn_DIPR (sn, addr);
	setSn_DPORT(sn, port);

	/* Update TX pointer according to data length */
	ptr = getSn_TX_WR(sn);
	ptr += len;
	setSn_TX_WR(sn, ptr);

	/* Send datas ! :) */
	setSn_CR(sn,Sn_CR_SEND);
	/* wait to process the command... */
	while(getSn_CR(sn))
		;

	/* Wait for the result */
	while(1)
    {
        tmp = getSn_IR(sn);
        if(tmp & Sn_IR_SENDOK)
        {
            setSn_IR(sn, Sn_IR_SENDOK);
            break;
        }
        else if(tmp & Sn_IR_TIMEOUT)
        {
            setSn_IR(sn, Sn_IR_TIMEOUT);
            return SOCKERR_TIMEOUT;
        }
    }
    return len;
}

/* make the common DHCP message */
u8 * makeDHCPMSG(dhcp_session *session)
{
	u32 addr;
	u32 offset;
	u32 xid;
	uint8_t* ptmp;
	uint8_t  i;
	RIP_MSG *pDHCPMSG;

	addr = WZTOE_TX | (session->socket << 18);
	offset = (getSn_TX_WR(session->socket) & 0x0FFF);
	pDHCPMSG = (RIP_MSG *)(addr + offset);

	/* Wait until enough memory available into TX buffer */
	while (getSn_TxMAX(session->socket) < sizeof(RIP_MSG))
	 ;

	xid = session->dhcp_xid;

	pDHCPMSG->op      = DHCP_BOOTREQUEST;
	pDHCPMSG->htype   = DHCP_HTYPE10MB;
	pDHCPMSG->hlen    = DHCP_HLENETHERNET;
	pDHCPMSG->hops    = DHCP_HOPS;
	ptmp              = (uint8_t*)(&pDHCPMSG->xid);
	*(ptmp+0)         = (uint8_t)((xid & 0xFF000000) >> 24);
	*(ptmp+1)         = (uint8_t)((xid & 0x00FF0000) >> 16);
	*(ptmp+2)         = (uint8_t)((xid & 0x0000FF00) >>  8);
	*(ptmp+3)         = (uint8_t)((xid & 0x000000FF) >>  0);   
	pDHCPMSG->secs    = DHCP_SECS;
	ptmp              = (uint8_t*)(&pDHCPMSG->flags);	
	*(ptmp+0)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0xFF00) >> 8);
	*(ptmp+1)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0x00FF) >> 0);

	pDHCPMSG->ciaddr[0] = 0;
	pDHCPMSG->ciaddr[1] = 0;
	pDHCPMSG->ciaddr[2] = 0;
	pDHCPMSG->ciaddr[3] = 0;

	pDHCPMSG->yiaddr[0] = 0;
	pDHCPMSG->yiaddr[1] = 0;
	pDHCPMSG->yiaddr[2] = 0;
	pDHCPMSG->yiaddr[3] = 0;

	pDHCPMSG->siaddr[0] = 0;
	pDHCPMSG->siaddr[1] = 0;
	pDHCPMSG->siaddr[2] = 0;
	pDHCPMSG->siaddr[3] = 0;

	pDHCPMSG->giaddr[0] = 0;
	pDHCPMSG->giaddr[1] = 0;
	pDHCPMSG->giaddr[2] = 0;
	pDHCPMSG->giaddr[3] = 0;

	pDHCPMSG->chaddr[0] = session->dhcp_chaddr[0];
	pDHCPMSG->chaddr[1] = session->dhcp_chaddr[1];
	pDHCPMSG->chaddr[2] = session->dhcp_chaddr[2];
	pDHCPMSG->chaddr[3] = session->dhcp_chaddr[3];
	pDHCPMSG->chaddr[4] = session->dhcp_chaddr[4];
	pDHCPMSG->chaddr[5] = session->dhcp_chaddr[5];

	for (i = 6; i < 16; i++)  pDHCPMSG->chaddr[i] = 0;
	for (i = 0; i < 64; i++)  pDHCPMSG->sname[i]  = 0;
	for (i = 0; i < 128; i++) pDHCPMSG->file[i]   = 0;

	// MAGIC_COOKIE
	pDHCPMSG->OPT[0] = (uint8_t)((MAGIC_COOKIE & 0xFF000000) >> 24);
	pDHCPMSG->OPT[1] = (uint8_t)((MAGIC_COOKIE & 0x00FF0000) >> 16);
	pDHCPMSG->OPT[2] = (uint8_t)((MAGIC_COOKIE & 0x0000FF00) >>  8);
	pDHCPMSG->OPT[3] = (uint8_t) (MAGIC_COOKIE & 0x000000FF) >>  0;
	
	return (u8 *)pDHCPMSG;
}

/* SEND DHCP DISCOVER */
void send_DHCP_DISCOVER(dhcp_session *session)
{
	const char hostname[] = DHCP_HOST_NAME;
	uint16_t i;
	uint8_t ip[4];
	uint16_t k = 0;
	RIP_MSG *pDHCPMSG;

	pDHCPMSG = (RIP_MSG *)makeDHCPMSG(session);

   k = 4;     // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
   
	// Option Request Param
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DISCOVER;
	
	// Client identifier
	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[0];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[1];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[2];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[3];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[4];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[5];
	
	// host name
	pDHCPMSG->OPT[k++] = hostName;
	pDHCPMSG->OPT[k++] = 0;          // fill zero length of hostname 
	for(i = 0 ; hostname[i] != 0; i++)
   	pDHCPMSG->OPT[k++] = hostname[i];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[3];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[4];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[5];
	pDHCPMSG->OPT[k - (i+3+1)] = i+3; // length of hostname

	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x06;	// length of request
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;

	// send broadcasting packet
	ip[0] = 255;
	ip[1] = 255;
	ip[2] = 255;
	ip[3] = 255;

#ifdef _DHCP_DEBUG_
	uart_puts("> Send DHCP_DISCOVER\r\n");
#endif

	dhcp_sendto(session->socket, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/* SEND DHCP REQUEST */
void send_DHCP_REQUEST(dhcp_session *session)
{
	const char hostname[] = DHCP_HOST_NAME;
	int i;
	uint8_t ip[4];
	uint16_t k = 0;
	RIP_MSG *pDHCPMSG;

	pDHCPMSG = (RIP_MSG *)makeDHCPMSG(session);

   if(session->state == STATE_DHCP_LEASED || session->state == STATE_DHCP_REREQUEST)
   {
   	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
   	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);
   	pDHCPMSG->ciaddr[0] = session->dhcp_my_ip[0];
   	pDHCPMSG->ciaddr[1] = session->dhcp_my_ip[1];
   	pDHCPMSG->ciaddr[2] = session->dhcp_my_ip[2];
   	pDHCPMSG->ciaddr[3] = session->dhcp_my_ip[3];
   	ip[0] = session->dhcp_sip[0];
   	ip[1] = session->dhcp_sip[1];
   	ip[2] = session->dhcp_sip[2];
   	ip[3] = session->dhcp_sip[3];   	   	   	
   }
   else
   {
   	ip[0] = 255;
   	ip[1] = 255;
   	ip[2] = 255;
   	ip[3] = 255;   	   	   	
   }
   
   k = 4;      // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
	
	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_REQUEST;

	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[0];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[1];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[2];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[3];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[4];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[5];

   if(ip[3] == 255)  // if(dchp_state == STATE_DHCP_LEASED || dchp_state == DHCP_REREQUEST_STATE)
   {
		pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = session->dhcp_my_ip[0];
		pDHCPMSG->OPT[k++] = session->dhcp_my_ip[1];
		pDHCPMSG->OPT[k++] = session->dhcp_my_ip[2];
		pDHCPMSG->OPT[k++] = session->dhcp_my_ip[3];
	
		pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = session->dhcp_sip[0];
		pDHCPMSG->OPT[k++] = session->dhcp_sip[1];
		pDHCPMSG->OPT[k++] = session->dhcp_sip[2];
		pDHCPMSG->OPT[k++] = session->dhcp_sip[3];
	}

	// host name
	pDHCPMSG->OPT[k++] = hostName;
	pDHCPMSG->OPT[k++] = 0; // length of hostname
	for(i = 0 ; hostname[i] != 0; i++)
   	pDHCPMSG->OPT[k++] = hostname[i];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[3];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[4];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[5];
	pDHCPMSG->OPT[k - (i+3+1)] = i+3; // length of hostname
	
	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x08;
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = performRouterDiscovery;
	pDHCPMSG->OPT[k++] = staticRoute;
	pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;

#ifdef _DHCP_DEBUG_
	uart_puts("> Send DHCP_REQUEST\r\n");
#endif
	
	dhcp_sendto(session->socket, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/* SEND DHCP DHCPDECLINE */
void send_DHCP_DECLINE(dhcp_session *session)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;
	RIP_MSG *pDHCPMSG;

	pDHCPMSG = (RIP_MSG *)makeDHCPMSG(session);

   k = 4;      // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
   
	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);

	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DECLINE;

	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[0];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[1];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[2];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[3];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[4];
	pDHCPMSG->OPT[k++] = session->dhcp_chaddr[5];

	pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = session->dhcp_my_ip[0];
	pDHCPMSG->OPT[k++] = session->dhcp_my_ip[1];
	pDHCPMSG->OPT[k++] = session->dhcp_my_ip[2];
	pDHCPMSG->OPT[k++] = session->dhcp_my_ip[3];

	pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = session->dhcp_sip[0];
	pDHCPMSG->OPT[k++] = session->dhcp_sip[1];
	pDHCPMSG->OPT[k++] = session->dhcp_sip[2];
	pDHCPMSG->OPT[k++] = session->dhcp_sip[3];

	pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;

	//send broadcasting packet
	ip[0] = 0xFF;
	ip[1] = 0xFF;
	ip[2] = 0xFF;
	ip[3] = 0xFF;

#ifdef _DHCP_DEBUG_
	uart_puts("\r\n> Send DHCP_DECLINE\r\n");
#endif

	dhcp_sendto(session->socket, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/* PARSE REPLY pDHCPMSG */
int8_t parseDHCPMSG(dhcp_session *session)
{
	uint16_t len;
	uint16_t socket_len;

	uint8_t * p;
	uint8_t * e;
	uint8_t type = 0;
	uint8_t opt_len;
	u8      *ptr;
	uint32_t offset;
	RIP_MSG *pDHCPMSG;

	pDHCPMSG = (RIP_MSG*)session->buffer;
   
	socket_len = getSn_RX_RSR(session->socket);
	if(socket_len < 8)
		return(0);

        offset = (getSn_RX_RD(session->socket) & 0x0FFF);
	ptr = (u8 *)( RXMEM_BASE | (session->socket << 18) );
	ptr += offset;
	/* Get the packet data length */
	len = ( (ptr[6] << 8) | ptr[7]);
	
	/* For debug only */
	if (socket_len < (len + 8))
	{
		uart_puts("DHCP: error socket_len < (len + 8)\r\n");
		return(0);
	}
	
	/* If packet comes from BOOTP server port ? */
	if ((ptr[4] != 0x00) || (ptr[5] != 0x43))
		goto drop_packet;
	
	pDHCPMSG = (RIP_MSG *)(ptr + 8);
	
   #ifdef _DHCP_DEBUG_   
      //printf("DHCP message : %d.%d.%d.%d(%d) %d received. \r\n",svr_addr[0],svr_addr[1],svr_addr[2], svr_addr[3],svr_port, len);
   #endif   
	// Compare mac address
	if ( (pDHCPMSG->chaddr[0] != session->dhcp_chaddr[0]) || (pDHCPMSG->chaddr[1] != session->dhcp_chaddr[1]) ||
	     (pDHCPMSG->chaddr[2] != session->dhcp_chaddr[2]) || (pDHCPMSG->chaddr[3] != session->dhcp_chaddr[3]) ||
	     (pDHCPMSG->chaddr[4] != session->dhcp_chaddr[4]) || (pDHCPMSG->chaddr[5] != session->dhcp_chaddr[5])   )
	{
		uart_puts("DHCP: bad MAC\r\n");
		goto drop_packet;
	}

	if (session->state == STATE_DHCP_DISCOVER)
	{
		/* If the session wait for a file */
		if (session->dhcp_file)
			strcpy((char *)session->dhcp_file, (char *)pDHCPMSG->file);

		/* Save the offered IP */
		session->dhcp_my_ip[0] = pDHCPMSG->yiaddr[0];
		session->dhcp_my_ip[1] = pDHCPMSG->yiaddr[1];
		session->dhcp_my_ip[2] = pDHCPMSG->yiaddr[2];
		session->dhcp_my_ip[3] = pDHCPMSG->yiaddr[3];

		session->dhcp_siaddr[0] = pDHCPMSG->siaddr[0];
		session->dhcp_siaddr[1] = pDHCPMSG->siaddr[1];
		session->dhcp_siaddr[2] = pDHCPMSG->siaddr[2];
		session->dhcp_siaddr[3] = pDHCPMSG->siaddr[3];
	}

	p = (uint8_t *)(&pDHCPMSG->op);
	p = p + 240;      // 240 = sizeof(RIP_MSG) + MAGIC_COOKIE size in RIP_MSG.opt - sizeof(RIP_MSG.opt)
	e = p + (len - 240);

	while ( p < e ) {

		switch ( *p ) {

			case endOption :
   				p = e;   // for break while(p < e)
   				break;
			case padOption :
   				p++;
   				break;
   			case dhcpMessageType :
   				p++;
   				p++;
   				type = *p++;
   				break;
   			case subnetMask :
   				p++;
   				p++;
   				session->dhcp_my_sn[0] = *p++;
   				session->dhcp_my_sn[1] = *p++;
   				session->dhcp_my_sn[2] = *p++;
   				session->dhcp_my_sn[3] = *p++;
   				break;
   			case routersOnSubnet :
   				p++;
   				opt_len = *p++;       
   				session->dhcp_my_gw[0] = *p++;
   				session->dhcp_my_gw[1] = *p++;
   				session->dhcp_my_gw[2] = *p++;
   				session->dhcp_my_gw[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dns :
   				p++;                  
   				opt_len = *p++;       
   				session->dhcp_my_dns[0] = *p++;
   				session->dhcp_my_dns[1] = *p++;
   				session->dhcp_my_dns[2] = *p++;
   				session->dhcp_my_dns[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dhcpIPaddrLeaseTime :
   			{
   			        u32 lease_time;
   				p++;
   				opt_len = *p++;
   				lease_time  = *p++;
   				lease_time  = (lease_time << 8) + *p++;
   				lease_time  = (lease_time << 8) + *p++;
   				lease_time  = (lease_time << 8) + *p++;
				#ifdef _DHCP_DEBUG_  
				lease_time = 10;
 				#endif
 				session->lease_time = lease_time;
   				break;
                        }
   			case dhcpServerIdentifier :
   				p++;
   				opt_len = *p++;
   				session->dhcp_sip[0] = *p++;
   				session->dhcp_sip[1] = *p++;
   				session->dhcp_sip[2] = *p++;
   				session->dhcp_sip[3] = *p++;
   				break;
   			default :
   				p++;
   				opt_len = *p++;
   				p += opt_len;
   				break;
		} // switch
	} // while

drop_packet:
	offset = getSn_RX_RD(session->socket);
	offset += (len + 8);
	/* Update RX pointer */
	setSn_RX_RD(session->socket, offset);
	/* Mark received datas as read */
        setSn_CR(session->socket, Sn_CR_RECV);
        while(getSn_CR(session->socket))
            ;

	return type;
}

uint8_t DHCP_run(dhcp_session *session)
{
	uint8_t  type;
	uint8_t  ret;

	if(session->state == STATE_DHCP_STOP)
		return DHCP_STOPPED;

	if(getSn_SR(session->socket) != SOCK_UDP)
		return DHCP_STOPPED;

	ret = DHCP_RUNNING;
	type = parseDHCPMSG(session);

	switch ( session->state ) {
		case STATE_DHCP_INIT     :
			session->dhcp_my_ip[0] = 0;
			session->dhcp_my_ip[1] = 0;
			session->dhcp_my_ip[2] = 0;
			session->dhcp_my_ip[3] = 0;
   		send_DHCP_DISCOVER(session);
   		session->state = STATE_DHCP_DISCOVER;
   		break;
		case STATE_DHCP_DISCOVER :
			if (type == DHCP_OFFER){
#ifdef _DHCP_DEBUG_
				uart_puts("> Receive DHCP_OFFER\r\n");
#endif
				send_DHCP_REQUEST(session);
				session->state = STATE_DHCP_REQUEST;
			}
			else
				ret = check_DHCP_timeout(session);
         break;

		case STATE_DHCP_REQUEST :
			if (type == DHCP_ACK) {

#ifdef _DHCP_DEBUG_
				uart_puts("> Receive DHCP_ACK\r\n");
#endif

      if (check_DHCP_leasedIP(session)) {
					// Network info assignment from DHCP
					default_ip_assign(session);
					reset_DHCP_timeout(session);
					session->state = STATE_DHCP_LEASED;
				} else {
					// IP address conflict occurred
					reset_DHCP_timeout(session);
					default_ip_conflict(session);
				    session->state = STATE_DHCP_INIT;
				}
			} else if (type == DHCP_NAK) {

#ifdef _DHCP_DEBUG_
				uart_puts("> Receive DHCP_NACK\r\n");
#endif

				reset_DHCP_timeout(session);
				session->state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout(session);
		break;

		case STATE_DHCP_LEASED :
		   ret = DHCP_IP_LEASED;
			if ((session->lease_time != INFINITE_LEASETIME) && ((session->lease_time/2) < session->tick_1s)) {
				
#ifdef _DHCP_DEBUG_
 				uart_puts("> Maintains the IP address \r\n");
#endif

				type = 0;
				session->old_ip[0] = session->dhcp_my_ip[0];
				session->old_ip[1] = session->dhcp_my_ip[1];
				session->old_ip[2] = session->dhcp_my_ip[2];
				session->old_ip[3] = session->dhcp_my_ip[3];
				
				session->dhcp_xid++;

				send_DHCP_REQUEST(session);

				reset_DHCP_timeout(session);

				session->state = STATE_DHCP_REREQUEST;
			}
		break;

		case STATE_DHCP_REREQUEST :
		   ret = DHCP_IP_LEASED;
			if (type == DHCP_ACK) {
				session->retry = 0;
				if (session->old_ip[0] != session->dhcp_my_ip[0] || 
				    session->old_ip[1] != session->dhcp_my_ip[1] ||
				    session->old_ip[2] != session->dhcp_my_ip[2] ||
				    session->old_ip[3] != session->dhcp_my_ip[3]) 
				{
					ret = DHCP_IP_CHANGED;
					default_ip_update(session);
               #ifdef _DHCP_DEBUG_
                  uart_puts(">IP changed.\r\n");
               #endif
					
				}
         #ifdef _DHCP_DEBUG_
            else uart_puts(">IP is continued.\r\n");
         #endif            				
				reset_DHCP_timeout(session);
				session->state = STATE_DHCP_LEASED;
			} else if (type == DHCP_NAK) {

#ifdef _DHCP_DEBUG_
				uart_puts("> Receive DHCP_NACK, Failed to maintain ip\r\n");
#endif

				reset_DHCP_timeout(session);

				session->state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout(session);
	   	break;
		default :
   		break;
	}

	return ret;
}

void DHCP_stop(dhcp_session *session)
{
	/* Send Close command to DHCP socket */
	setSn_CR(session->socket, Sn_CR_CLOSE);
	/* wait to process the command... */
	while( getSn_CR(session->socket) )
		;
	/* Clear all interrupt of the socket. */
	setSn_IR(session->socket, 0xFF);
	/* Wait until socket is closed */
	while(getSn_SR(session->socket) != SOCK_CLOSED)
		;

	session->state = STATE_DHCP_STOP;
}

uint8_t check_DHCP_timeout(dhcp_session *session)
{
	uint8_t ret = DHCP_RUNNING;
	
	if (session->retry < MAX_DHCP_RETRY) {
		if (session->tick_next < session->tick_1s) {

			switch ( session->state ) {
				case STATE_DHCP_DISCOVER :
					//uart_puts("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
					send_DHCP_DISCOVER(session);
				break;
		
				case STATE_DHCP_REQUEST :
					//uart_puts("<<timeout>> state : STATE_DHCP_REQUEST\r\n");

					send_DHCP_REQUEST(session);
				break;

				case STATE_DHCP_REREQUEST :
					//uart_puts("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");
					
					send_DHCP_REQUEST(session);
				break;
		
				default :
				break;
			}

			session->tick_1s = 0;
			session->tick_next = session->tick_1s + DHCP_WAIT_TIME;
			session->retry ++;
		}
	} else { // timeout occurred

		switch(session->state) {
			case STATE_DHCP_DISCOVER:
				session->state = STATE_DHCP_INIT;
				ret = DHCP_FAILED;
				break;
			case STATE_DHCP_REQUEST:
			case STATE_DHCP_REREQUEST:
				send_DHCP_DISCOVER(session);
				session->state = STATE_DHCP_DISCOVER;
				break;
			default :
				break;
		}
		reset_DHCP_timeout(session);
	}
	return ret;
}

int8_t check_DHCP_leasedIP(dhcp_session *session)
{
	uint8_t tmp;
	int32_t ret;

	//WIZchip RCR value changed for ARP Timeout count control
	tmp = getRCR();
	setRCR(0x03);

	// IP conflict detection : ARP request - ARP reply
	// Broadcasting ARP Request for check the IP conflict using UDP sendto() function
  //printf("%d %d %d %d\r\n", session->dhcp_my_ip[0], session->dhcp_my_ip[1], session->dhcp_my_ip[2], session->dhcp_my_ip[3]);

ret = dhcp_sendto(session->socket, (uint8_t *)"CHECK_IP_CONFLICT", 17, session->dhcp_my_ip, 5000);

	// RCR value restore
	setRCR(tmp);
	if(ret == SOCKERR_TIMEOUT) {
  // UDP send Timeout occurred : allocated IP address is unique, DHCP Success

#ifdef _DHCP_DEBUG_
		uart_puts("\r\n> Check leased IP - OK\r\n");
#endif

		return 1;
	} else {
		// Received ARP reply or etc : IP address conflict occur, DHCP Failed
		send_DHCP_DECLINE(session);

		ret = session->tick_1s;
		while((session->tick_1s - ret) < 2)
		   ;   // wait for 1s over; wait to complete to send DECLINE message;

		return 0;
	}
}	

void DHCP_init(dhcp_session *session)
{
	uint8_t zeroip[4] = {0,0,0,0};

   getSHAR(session->dhcp_chaddr);
   if((session->dhcp_chaddr[0] | session->dhcp_chaddr[1]  | session->dhcp_chaddr[2] | session->dhcp_chaddr[3] | session->dhcp_chaddr[4] | session->dhcp_chaddr[5]) == 0x00)
   {
      // assing temporary mac address, you should be set SHAR before call this function. 
      session->dhcp_chaddr[0] = 0x00;
      session->dhcp_chaddr[1] = 0x08;
      session->dhcp_chaddr[2] = 0xdc;      
      session->dhcp_chaddr[3] = 0x00;
      session->dhcp_chaddr[4] = 0x00;
      session->dhcp_chaddr[5] = 0x00; 
      setSHAR(session->dhcp_chaddr);     
   }

	session->dhcp_xid = 0x12345678;

	// WIZchip Netinfo Clear
	setSIPR(zeroip);
	setSIPR(zeroip);
	setGAR(zeroip);

	reset_DHCP_timeout(session);
	session->state = STATE_DHCP_INIT;
	session->retry = 0;
	session->tick_next  = DHCP_WAIT_TIME;
	session->lease_time = INFINITE_LEASETIME;
	session->dhcp_file  = 0;
	
	/* Configure DHCP socket as UDP */
	setSn_MR  (session->socket, Sn_MR_UDP);
	/* Set local port */
	setSn_PORT(session->socket, DHCP_CLIENT_PORT);

	/* Send "Open" command to the socket */
	setSn_CR  (session->socket, Sn_CR_OPEN);
	while( getSn_CR(session->socket) )
		;
	/* Wait until socket initialisation complete */
	while(getSn_SR(session->socket) == SOCK_CLOSED)
		;
}

/* Rset the DHCP timeout count and retry count. */
void reset_DHCP_timeout(dhcp_session *session)
{
	session->tick_1s = 0;
	session->tick_next = DHCP_WAIT_TIME;
	session->retry = 0;
}

void DHCP_time_handler(dhcp_session *session)
{
	session->tick_1s ++;
}
