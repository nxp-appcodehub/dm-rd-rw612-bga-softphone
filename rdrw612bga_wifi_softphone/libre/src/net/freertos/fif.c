/**
 * @file freertos/if.c  FreeRTOS network interface using lwIP
 */

#include <re_types.h>
#include <re_fmt.h>
#include <re_sa.h>
#include <re_net.h>
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#define DEBUG_MODULE "freertos_if"
#define DEBUG_LEVEL 5
#include <re_dbg.h>

/**
 * Enumerate all network interfaces using lwIP
 *
 * @param ifh Interface handler
 * @param arg Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int net_if_list(net_ifaddr_h *ifh, void *arg)
{
    struct netif *netif;
    struct sa sa;
    char ifname[IFNAMSIZ];
    int err = 0;

    if (!ifh)
        return EINVAL;

    /* Iterate through all network interfaces */
    NETIF_FOREACH(netif) {
        /* Skip interfaces that are down */
        if (!netif_is_up(netif))
            continue;

        /* Get interface name */
        snprintf(ifname, sizeof(ifname), "%c%c%d",
                 netif->name[0], netif->name[1], netif->num);

        /* Handle IPv4 address */
        if (!ip4_addr_isany(netif_ip4_addr(netif))) {
            sa_init(&sa, AF_INET);
            sa.u.in.sin_addr.s_addr = ip4_addr_get_u32(netif_ip4_addr(netif));

            if (ifh(ifname, &sa, arg))
                break;
        }

#if LWIP_IPV6
        /* Handle IPv6 addresses */
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
                const ip6_addr_t *ip6 = netif_ip6_addr(netif, i);

                sa_init(&sa, AF_INET6);
                memcpy(&sa.u.in6.sin6_addr, ip6->addr, 16);
                sa.u.in6.sin6_scope_id = netif_get_index(netif);

                if (ifh(ifname, &sa, arg))
                    goto out;
            }
        }
#endif
    }

out:
    return err;
}

/**
 * Get IP address for a given network interface
 *
 * @param ifname  Network interface name
 * @param af      Address Family
 * @param ip      Returned IP address
 *
 * @return 0 if success, otherwise errorcode
 */
int net_if_getaddr(const char *ifname, int af, struct sa *ip)
{
    struct netif *netif;
    char name_buf[IFNAMSIZ];

    if (!ip)
        return EINVAL;

    /* Find the specified interface */
    NETIF_FOREACH(netif) {
        snprintf(name_buf, sizeof(name_buf), "%c%c%d",
                 netif->name[0], netif->name[1], netif->num);

        if (ifname && strcmp(ifname, name_buf) != 0)
            continue;

        if (!netif_is_up(netif))
            continue;

        if (af == AF_INET && !ip4_addr_isany(netif_ip4_addr(netif))) {
            sa_init(ip, AF_INET);
            ip->u.in.sin_addr.s_addr = ip4_addr_get_u32(netif_ip4_addr(netif));
            return 0;
        }

#if LWIP_IPV6
        if (af == AF_INET6) {
            for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
                    const ip6_addr_t *ip6 = netif_ip6_addr(netif, i);

                    /* Skip link-local if we have global addresses */
                    if (ip6_addr_islinklocal(ip6))
                        continue;

                    sa_init(ip, AF_INET6);
                    memcpy(&ip->u.in6.sin6_addr, ip6->addr, 16);
                    ip->u.in6.sin6_scope_id = netif_get_index(netif);
                    return 0;
                }
            }
        }
#endif

        /* If no specific interface requested, return first valid */
        if (!ifname)
            break;
    }

    return ENODEV;
}

/**
 * Get IP address for a given network interface
 *
 * @param ifname  Network interface name
 * @param af      Address Family
 * @param ip      Returned IP address
 *
 * @return 0 if success, otherwise errorcode
 *
 * @deprecated Works for IPv4 only
 */
int net_if_getaddr4(const char *ifname, int af, struct sa *ip)
{
    extern struct netif gnetif;
    sa_set_in(ip, ntohl(gnetif.ip_addr.u_addr.ip4.addr), 0);
	return 0;
}
