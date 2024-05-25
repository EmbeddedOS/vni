/**
 * @file    vni.c
 * @author  LaVa264 (congnt264@gmail.com)
 * @brief   A kernel module to simulate a network interface. This acts like a
 *          loopback device that routes every transmitted packets immediately
 *          into the receiver on the same channel.
 *
 * @version 0.1
 * @date 2024-05-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/net_tstamp.h>
#include <net/sock.h>
#include "protocol.h"

/* Public define -------------------------------------------------------------*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LaVa264");
MODULE_DESCRIPTION("Virtual Network Interface.");

#define __pr_info(fmt, arg...) pr_info("%s(): " fmt "\n", __FUNCTION__, ##arg)
#define __pr_err(fmt, arg...) pr_err("%s(): " fmt "\n", __FUNCTION__, ##arg)

#define DEVICE_NAME "vni"

static int vni_netdev_init(struct net_device *dev);
static int vni_netdev_open(struct net_device *dev);
static int vni_netdev_start_xmit(struct sk_buff *skb,
                                 struct net_device *dev);
struct vni_data
{
};

/**
 * @brief   This structure represent a network interface.
 *
 * @features:           Currently active device features.
 * @hard_header_len:    Maximum hardware header length.
 * @min_header_len:     Minimum hardware header length.
 * @addr_len:           Hardware address length.
 * @type:               Interface hardware type. For more hw identifiers,
 *                      look into: `include/uapi/linux/if_arp.h`.
 * @flags:              Interface flags.
 *
 * @netdev_ops:     Includes several pointers to callbacks, if one wants to
 *                  override the `ndo_*()` functions.
 * @ethtool_ops:    Management operations.
 * @header_ops:     Includes callbacks for creating, parsing, caching, etc. of
 *                  Layer 2 headers.
 */
struct net_device *g_p_dev = NULL;

/**
 * @brief   This structure defines the management hooks for network devices.
 *
 * int (*ndo_init)(struct net_device *dev);
 *          This function is called once when a network is registered. The
 *          driver can use this for any late stage initialization.
 *
 * int (*ndo_open)(struct net_device *dev);
 *          This function is called when a network transitions to the up state.
 *
 * int (*ndo_stop)(struct net_device *dev);
 *          This function is called when a network device transitions to the
 *          down state.
 *
 * int (*ndo_start_xmit)(struct sk_buff *skb,
 *                       struct net_device *dev);
 *          Called when a packet NEEDS to be transmitted.
 *          Required; cannot be NULL.
 */
static const struct net_device_ops vni_ops = {
    .ndo_init = &vni_netdev_init,
    .ndo_start_xmit = &vni_netdev_start_xmit,
    .ndo_open = &vni_netdev_open};

/**
 * @brief   This callback is called when allocating to finish initializing.
 *
 * @param   dev - Network Interface.
 */
static void vni_setup(struct net_device *dev)
{
    __pr_info("Called.");

    dev->mtu = 64 * 1024;              /* 64 bytes. */
    dev->hard_header_len = HEADER_LEN; /* 14 bytes. */
    dev->min_header_len = HEADER_LEN;  /* 14 bytes. */
    dev->addr_len = ADDR_LEN;          /* 6 bytes. */

    dev->netdev_ops = &vni_ops; /* Required, cannot be NULL. */
}

static int __init vni_init(void)
{
    int result = 0;

    /* 1. Allocate virtual device. */
    g_p_dev = alloc_netdev(sizeof(struct vni_data),
                           DEVICE_NAME "%d",
                           NET_NAME_UNKNOWN,
                           vni_setup);
    if (!g_p_dev)
    {
        __pr_err("Failed to allocate %s.", DEVICE_NAME);
        result = -ENOMEM;
        goto alloc_netdev_failure;
    }

    __pr_info("Allocated netdev.");

    /* 2. Register the interface with kernel. Expose to user space. */
    result = register_netdev(g_p_dev);
    if (result)
    {
        __pr_err("Failed to register %s.", DEVICE_NAME);
        goto register_netdev_failure;
    }

    __pr_info("Registered netdev.");

    return 0;

register_netdev_failure:
    free_netdev(g_p_dev);

alloc_netdev_failure:
    return result;
}

static void __exit vni_exit(void)
{
    __pr_info("called.");
    unregister_netdev(g_p_dev);
    free_netdev(g_p_dev);
}

module_init(vni_init);
module_exit(vni_exit);

static int vni_netdev_init(struct net_device *dev)
{
    __pr_info("Called.");
    return 0;
}

static int vni_netdev_start_xmit(struct sk_buff *skb,
                                 struct net_device *dev)
{
    __pr_info("User request to sent: %s", skb->data);

    // skb_orphan(skb);
	// skb_tx_timestamp(skb);

	// /* do not fool net_timestamp_check() with various clock bases */
	// //skb_clear_tstamp(skb);
    // /* Before queueing this packet to __netif_rx(),
    //  * make sure dst is refcounted.
    //  */
    // skb_dst_force(skb);

    if (likely(netif_rx(skb) != NET_RX_SUCCESS))
    {
        __pr_err("Failed to push packet to the RX queue!");
    }

    return NETDEV_TX_OK;
}

static int vni_netdev_open(struct net_device *dev)
{
    __pr_info("Called.");

    /* Set MAC address. */
    memcpy(dev->dev_addr, ADDR_STR, ADDR_LEN);

    /* Start TX queue that allow upper layers to call the device
     * hard_start_xmit() routine. */
    netif_start_queue(dev);
    return 0;
}
