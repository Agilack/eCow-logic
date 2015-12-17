/**
 * eCow-logic - Bootloader
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
#include "net_tftp.h"
#include "libc.h"
#include "W7500x_wztoe.h"

#define TFTP_SOCK 3
#define TFTP_PORT 12345

static void tftp_ack(u16 n);

void tftp_init(tftp *session)
{
    /* Configure TFTP socket as UDP */
    setSn_MR  (TFTP_SOCK, Sn_MR_UDP);
    /* Set local port */
    setSn_PORT(TFTP_SOCK, TFTP_PORT);
    /* Send "Open" command to the socket */
    setSn_CR  (TFTP_SOCK,Sn_CR_OPEN);
    while( getSn_CR(TFTP_SOCK) )
        ;
    /* Wait until socket initialisation complete */
    while(getSn_SR(TFTP_SOCK) == SOCK_CLOSED)
        ;

    if (session)
    {
        session->state = 0;
        session->lastblock = 0;
    }
}

int tftp_run(tftp *session)
{
    int len;
    u8 *pkt;
    u16 blocknum;
    
    pkt = session->buffer;
    
    switch(session->state)
    {
        case 0:
            /* Fill request packet */
            pkt[0] = 0x00; pkt[1] = 0x01;
            strcpy((char *)&pkt[2], "ecow.upd");
            strcpy((char *)&pkt[11], (char *)"octet");
            pkt[17] = 0; pkt[18] = 0; pkt[19] = 0;
            /* Set destination (server) address */
            setSn_DIPR (TFTP_SOCK, session->server);
            /* Set destination port */
            setSn_DPORT(TFTP_SOCK, 69);
            /* Send datas */
            wiz_send_data(TFTP_SOCK, pkt, 17);
            setSn_CR(TFTP_SOCK, Sn_CR_SEND);
            while( getSn_CR(TFTP_SOCK) )
                ;
            session->state = 1;
            break;
        
        /* Wait server response */
        case 1:
            len = getSn_RX_RSR(TFTP_SOCK);
            if (len == 0)
                break;
            wiz_recv_data(TFTP_SOCK, pkt, len);
            /* Mark received datas as read */
            setSn_CR(TFTP_SOCK, Sn_CR_RECV);
            while(getSn_CR(TFTP_SOCK))
                ;
            if (pkt[9] == 0x03)
            {
                /* Update destination port */
                setSn_DPORT(TFTP_SOCK, (pkt[4] << 8 | pkt[5]));
                /* Send ack for this packet */
                tftp_ack(pkt[11]);
                session->lastblock = pkt[11];
                session->length = len;
                session->state = 2;
            }
            else if (pkt[9] == 5)
            {
                session->state = 99;
            }
            break;
        
        /* Data transfer ... */
        case 2:
            len = getSn_RX_RSR(TFTP_SOCK);
            if (len == 0)
                break;
            wiz_recv_data(TFTP_SOCK, pkt, len);
            /* Mark received datas as read */
            setSn_CR(TFTP_SOCK, Sn_CR_RECV);
            while(getSn_CR(TFTP_SOCK))
                ;
            blocknum = (pkt[10] << 8) | pkt[11];
            
            /* Send ack for this packet */
            tftp_ack(blocknum);
            
            if(blocknum > session->lastblock)
            {
                session->length = len;
                session->lastblock = blocknum;
            }
            else
                session->length = 0;
            
            if (len != 524)
                session->state = 3;
            break;
        
        /* Download complete */
        case 3:
            break;
    }
    return(0);
}

static void tftp_ack(u16 n)
{
    u8 pkt[16];
    
    /* Set TFTP opcode 04 (ACK) */
    pkt[0] = 0x00;
    pkt[1] = 0x04;
    /* Copy the block number */
    pkt[2] = (n >> 8) & 0xFF;
    pkt[3] = (n & 0xFF);
    
    /* Send datas */
    wiz_send_data(TFTP_SOCK, pkt, 4);
    setSn_CR(TFTP_SOCK, Sn_CR_SEND);
    while( getSn_CR(TFTP_SOCK) )
        ;
}
