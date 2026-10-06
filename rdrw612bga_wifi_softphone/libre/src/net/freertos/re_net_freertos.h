/**
 * @file re_net_freertos.h  Network interface for FreeRTOS + lwIP
 */

#ifndef SRC_NET_FREERTOS_RE_NET_FREERTOS_H_
#define SRC_NET_FREERTOS_RE_NET_FREERTOS_H_

/* FreeRTOS and lwIP includes */
#include "FreeRTOS.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"

/* Map lwIP types to re_net types */
#define re_sock_t int
#define RE_BAD_SOCK -1
#define RE_ERRNO_SOCK errno

/* Socket options compatibility */
#ifndef SO_REUSEADDR
#define SO_REUSEADDR 0x0004
#endif

#ifndef SO_REUSEPORT
#define SO_REUSEPORT 0x0200
#endif

/* Address family compatibility */
#ifndef AF_UNSPEC
#define AF_UNSPEC 0
#endif

/* Protocol compatibility */
#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif

#ifndef IPPROTO_TCP
#define IPPROTO_TCP 6
#endif

/* IPv6 socket options */
#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif

/* Interface flags for FreeRTOS/lwIP */
#define IFF_UP          0x1
#define IFF_BROADCAST   0x2
#define IFF_LOOPBACK    0x8
#define IFF_RUNNING     0x40
#define IFF_MULTICAST   0x1000

/* Maximum interface name length */
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#endif /* SRC_NET_FREERTOS_RE_NET_FREERTOS_H_ */
