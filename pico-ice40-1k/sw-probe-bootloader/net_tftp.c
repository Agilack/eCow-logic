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
#include "uart.h"

#define TFTP_SOCK 3
#define TFTP_PORT 12345

extern uint8_t DHCP_server_ip[4];

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
    }
}

int tftp_run(tftp *session)
{
    int len;
    u8 dst[4];
    u8 pkt[512];
    int i;
    
    switch(session->state)
    {
        case 0:
            /* Fill request packet */
            pkt[0] = 0x00; pkt[1] = 0x01;
            strcpy((char *)&pkt[2], "ecow.upd");
            strcpy((char *)&pkt[11], (char *)"octet");
            pkt[17] = 0; pkt[18] = 0; pkt[19] = 0;
            /* */
            dst[0] = DHCP_server_ip[0]; dst[1] = DHCP_server_ip[1];
            dst[2] = DHCP_server_ip[2]; dst[3] = DHCP_server_ip[3];
            setSn_DIPR (TFTP_SOCK, dst);
            /* Set destination port */
            setSn_DPORT(TFTP_SOCK, 69);
            /* Send datas */
            wiz_send_data(TFTP_SOCK, pkt, 17);
            setSn_CR(TFTP_SOCK, Sn_CR_SEND);
            while( getSn_CR(TFTP_SOCK) )
                ;
            session->state = 1;
            break;
        
        /* */
        case 1:
            len = getSn_RX_RSR(TFTP_SOCK);
            if (len == 0)
                break;
            wiz_recv_data(TFTP_SOCK, pkt, len);
            for (i = 0; i < len; i++)
            {
                uart_puthex8(pkt[i]);
                uart_putc(' ');
            }
            uart_puts("\r\n");
            /* Mark received datas as read */
            setSn_CR(TFTP_SOCK, Sn_CR_RECV);
            while(getSn_CR(TFTP_SOCK))
                ;
            if (pkt[9] == 5)
            {
                uart_puts((char *)&pkt[12]);
                session->state = 99;
            }
            else
                session->state = 2;
            break;
    }
    return(0);
}
