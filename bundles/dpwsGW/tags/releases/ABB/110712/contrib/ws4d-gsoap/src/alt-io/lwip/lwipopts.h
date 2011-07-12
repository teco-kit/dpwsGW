/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define LWIP_TIMEVAL_PRIVATE 0

#define LWIP_DBG_MIN_LEVEL 0
#define LWIP_COMPAT_SOCKETS 1
#define TAPIF_DEBUG      LWIP_DBG_ON
#define TUNIF_DEBUG      LWIP_DBG_OFF
#define UNIXIF_DEBUG     LWIP_DBG_OFF
#define DELIF_DEBUG      LWIP_DBG_OFF
#define SIO_FIFO_DEBUG   LWIP_DBG_OFF
#define TCPDUMP_DEBUG    LWIP_DBG_ON

#define PPP_DEBUG        LWIP_DBG_OFF
#define MEM_DEBUG        LWIP_DBG_OFF
#define MEMP_DEBUG       LWIP_DBG_OFF
#define PBUF_DEBUG       LWIP_DBG_OFF
#define API_LIB_DEBUG    LWIP_DBG_ON
#define API_MSG_DEBUG    LWIP_DBG_ON
#define TCPIP_DEBUG      LWIP_DBG_ON
#define NETIF_DEBUG      LWIP_DBG_ON
#define SOCKETS_DEBUG    LWIP_DBG_ON
#define DEMO_DEBUG       LWIP_DBG_ON
#define IP_DEBUG         LWIP_DBG_ON
#define IP_REASS_DEBUG   LWIP_DBG_ON
#define RAW_DEBUG        LWIP_DBG_ON
#define ICMP_DEBUG       LWIP_DBG_ON
#define UDP_DEBUG        LWIP_DBG_ON
#define TCP_DEBUG        LWIP_DBG_ON
#define TCP_INPUT_DEBUG  LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
#define TCP_RTO_DEBUG    LWIP_DBG_ON
#define TCP_CWND_DEBUG   LWIP_DBG_ON
#define TCP_WND_DEBUG    LWIP_DBG_ON
#define TCP_FR_DEBUG     LWIP_DBG_ON
#define TCP_QLEN_DEBUG   LWIP_DBG_ON
#define TCP_RST_DEBUG    LWIP_DBG_ON

extern unsigned char debug_flags;
#define LWIP_DBG_TYPES_ON debug_flags

#define NO_SYS                   0
#define LWIP_SOCKET              (NO_SYS==0)
#define LWIP_NETCONN             (NO_SYS==0)


/* ---------- Memory options ---------- */
#define MEM_ALIGNMENT            4
#define MEM_SIZE                 10240
#define MEMP_NUM_PBUF            16
#define MEMP_NUM_RAW_PCB         3
#define MEMP_NUM_UDP_PCB         4
#define MEMP_NUM_TCP_PCB         5
#define MEMP_NUM_TCP_PCB_LISTEN  8
#define MEMP_NUM_TCP_SEG         16
#define MEMP_NUM_SYS_TIMEOUT     16
#define MEMP_NUM_NETBUF          2
#define MEMP_NUM_NETCONN         10
#define MEMP_NUM_TCPIP_MSG_API   16
#define MEMP_NUM_TCPIP_MSG_INPKT 16

/* ---------- Pbuf options ---------- */
#define PBUF_POOL_SIZE           100
#define PBUF_POOL_BUFSIZE        128
#define PBUF_LINK_HLEN           16

#define SYS_LIGHTWEIGHT_PROT     1

/* ---------- TCP options ---------- */
#define LWIP_TCP                 1
#define TCP_TTL                  255
#define TCP_QUEUE_OOSEQ          1
#define TCP_MSS                  1024
#define TCP_SND_BUF              2048
#define TCP_SND_QUEUELEN         (4 * TCP_SND_BUF/TCP_MSS)
#define TCP_SNDLOWAT             (TCP_SND_BUF/2)
#define TCP_WND                  8096
#define TCP_MAXRTX               12
#define TCP_SYNMAXRTX            4

/* ---------- ARP options ---------- */
#define LWIP_ARP                 1
#define ARP_TABLE_SIZE           10
#define ARP_QUEUEING             1

/* ---------- IP options ---------- */
#define IP_FORWARD               1
#define IP_REASSEMBLY            1
#define IP_REASS_MAX_PBUFS       10
#define MEMP_NUM_REASSDATA       10
#define IP_FRAG           1

/* ---------- ICMP options ---------- */
#define ICMP_TTL                 255

/* ---------- DHCP options ---------- */
#define LWIP_DHCP                0
#define DHCP_DOES_ARP_CHECK      (LWIP_DHCP)

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP              0

/* ---------- UDP options ---------- */
#define LWIP_UDP                 1
#define UDP_TTL                  255

/* ---------- RAW options ---------- */
#define LWIP_RAW                 1
#define RAW_TTL                  255

/* ---------- IGMP options ---------- */
#define LWIP_IGMP                1

/* ---------- Statistics options ---------- */
#define LWIP_STATS               1

/* ---------- PPP options ---------- */
#define PPP_SUPPORT              0

#endif /* __LWIPOPTS_H__ */
