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
#include "net_tftp.h"
#include "libc.h"
#include "W7500x_wztoe.h"

#define TFTP_SOCK 3

void tftp_init(tftp *session)
{
    /* Configure TFTP socket as UDP */
    setSn_MR  (TFTP_SOCK, Sn_MR_UDP);
    /* Set local port */
    setSn_PORT(TFTP_SOCK, session->port);
    /* Send "Open" command to the socket */
    setSn_CR  (TFTP_SOCK,Sn_CR_OPEN);
    while( getSn_CR(TFTP_SOCK) )
        ;
    /* Wait until socket initialisation complete */
    while(getSn_SR(TFTP_SOCK) == SOCK_CLOSED)
        ;

    session->state = 0;
    session->lastblock = 0;
    session->filename  = 0;
    session->data = 0;
    session->timestamp = 0x2000;
}

int tftp_run(tftp *session)
{
    int len, totlen;
    u8 *pkt;
    u16 blocknum;
    u16 offset;
    u32 addr;
    
    switch(session->state)
    {
        case 0:
            /* The file name must be set (!) */
            if (session->filename == 0)
            {
                session->state = 99;
                break;
            }
            offset = getSn_TX_RD(TFTP_SOCK);
            offset &= 0x0FFF;
            setSn_TX_RD(TFTP_SOCK, offset);
            
            offset = (getSn_TX_WR(TFTP_SOCK) & 0x0FFF);
            addr = WZTOE_TX | (TFTP_SOCK << 18);
            pkt = (u8 *)(addr + offset);
            /* Fill request packet */
            pkt[0] = 0x00; pkt[1] = 0x01;
            strcpy((char *)&pkt[2], session->filename);
            len = 2 + strlen(session->filename) + 1; /* +1 for final \0 */
            strcpy((char *)&pkt[len], (char *)"octet");
            len += 6;
            /* Set destination (server) address */
            setSn_DIPR (TFTP_SOCK, session->server);
            /* Set destination port */
            setSn_DPORT(TFTP_SOCK, 69);
            /* Send datas */
            offset += len;
            setSn_TX_WR(TFTP_SOCK, offset);
            setSn_CR(TFTP_SOCK, Sn_CR_SEND);
            while( getSn_CR(TFTP_SOCK) )
                ;
            session->state = 1;
            break;
        
        /* Wait server response */
        case 1:
            session->timestamp --;
            totlen = getSn_RX_RSR(TFTP_SOCK);
            if (totlen < 8)
            {
                if (session->timestamp == 0)
                    session->state = 98;
                break;
            }
            offset = (getSn_RX_RD(TFTP_SOCK) & 0x0FFF);
            addr = WZTOE_RX | (TFTP_SOCK << 18);
            pkt = (u8 *)(addr + offset);
            /* Get the packet data length */
            len = ( (pkt[6] << 8) | pkt[7]);
            /* Verify that packet is fully received */
            if (totlen < (len + 8))
                break;
            
            if (pkt[9] == 0x03)
            {
                /* Update destination port */
                setSn_DPORT(TFTP_SOCK, (pkt[4] << 8 | pkt[5]));
                /* Update session */
                session->lastblock = pkt[11];
                session->length = len;
                session->data   = (pkt + 8);
                session->state = 2;
            }
            else if (pkt[9] == 5)
            {
                /* Drop the received packet */
                offset += (len + 8);
                setSn_RX_RD(TFTP_SOCK, offset);
                setSn_CR(TFTP_SOCK, Sn_CR_RECV);
                while(getSn_CR(TFTP_SOCK))
                    ;
                /* Set session in error state */
                session->state = 99;
            }
            break;
        
        /* Data transfer ... */
        case 2:
            totlen = getSn_RX_RSR(TFTP_SOCK);
            if (totlen == 0)
                break;
            offset = (getSn_RX_RD(TFTP_SOCK) & 0x0FFF);
            addr = WZTOE_RX | (TFTP_SOCK << 18);
            pkt = (u8 *)(addr + offset);
            /* Get the packet data length */
            len = ( (pkt[6] << 8) | pkt[7]);
            /* Verify that packet is fully received */
            if (totlen < (len + 8))
                break;
            
            blocknum = (pkt[10] << 8) | pkt[11];
            
            session->length = len;
            
            if(blocknum > session->lastblock)
            {
                session->data   = (pkt + 8);
                session->lastblock = blocknum;
            }
            else
            {
                tftp_ack(session);
                session->length = 0;
            }
            
            if (len != 516)
                session->state = 3;
            break;
        
        /* Download complete */
        case 3:
            break;
    }
    return(0);
}

void tftp_ack(tftp *session)
{
    u32 offset;
    u8  pkt[16];
    
    if (session->data)
    {
        /* Read (again) the current RX pointer */
        offset = getSn_RX_RD(TFTP_SOCK);
        offset += (session->length + 8);
        offset &= 0xFFFF;
        /* Update RX pointer */
        setSn_RX_RD(TFTP_SOCK, offset);
        /* Mark received datas as read */
        setSn_CR(TFTP_SOCK, Sn_CR_RECV);
        while(getSn_CR(TFTP_SOCK))
            ;
        session->data = 0;
    }

    /* Set TFTP opcode 04 (ACK) */
    pkt[0] = 0x00;
    pkt[1] = 0x04;
    /* Copy the block number */
    pkt[2] = (session->lastblock >> 8) & 0xFF;
    pkt[3] = (session->lastblock & 0xFF);
    
    /* Send datas */
    wiz_send_data(TFTP_SOCK, pkt, 4);
    setSn_CR(TFTP_SOCK, Sn_CR_SEND);
    while( getSn_CR(TFTP_SOCK) )
        ;
}

void tftp_stop(tftp *session)
{
    (void)session;
    return;
}
