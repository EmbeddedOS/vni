#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LaVa264");
MODULE_DESCRIPTION("Virtual Network Interface.");

#define __pr_info(fmt, arg...)      pr_info("%s(): " fmt, __FUNCTION__, ##arg)
#define __pr_err(fmt, arg...)       pr_err("%s(): " fmt, __FUNCTION__, ##arg)

#define DEVICE_NAME                 "vni"

static int vni_netdev_init(struct net_device *dev);

struct vni_data
{
    
};

struct net_device *g_p_dev = NULL;

static const struct net_device_ops vni_ops = {
    .ndo_init = &vni_netdev_init
};



/**
 * @brief This callback is called when allocating to finish initializing.
 * 
 * @param dev - Network Interface.
 */
static void vni_setup(struct net_device *dev)
{
    __pr_info("called.");
    //dev->netdev_ops = &vni_ops;
    ether_setup(dev);
}

static int __init vni_init(void)
{
    int result = 0;

    /* 1. Allocate virtual device. */
    g_p_dev = alloc_netdev(sizeof(struct vni_data),
                         DEVICE_NAME,
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
    __pr_info("called.");
    return 0;
}
