#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "protocol.h"

#define INTERFACE_MAX_LENGTH 20
#define HEADER "00000000000000"   /* Dummy 14 bytes for header. */
#define DATA "Im here"

int main(int argc, char **argv)
{
    char interface[INTERFACE_MAX_LENGTH] = {0};
    int res = 0;
    int fd = 0;
    struct ifreq interface_idx = {0};
    struct ifreq interface_addr = {0};
    struct sockaddr_ll socket_address = {0};

    if (argc < 2)
    {
        printf("Please specify interface name!\n");
        exit(EXIT_FAILURE);
    }

    strncpy(interface, argv[1], INTERFACE_MAX_LENGTH);

    /* 1. Create raw socket layer 2. */
    fd = socket(AF_PACKET, SOCK_RAW, 0);
    if (fd < 0)
    {
        printf("Cannot create socket: -%d\n", errno);
        exit(EXIT_FAILURE);
    }

    /* 2. Get the interface index. */
    strncpy(interface_idx.ifr_name, interface, INTERFACE_MAX_LENGTH);
    if (ioctl(fd, SIOCGIFINDEX, &interface_idx) < 0)
    {
        printf("Cannot get interface index: -%d\n", errno);
        close(fd);
        exit(EXIT_FAILURE);
    }

    socket_address.sll_ifindex = interface_idx.ifr_ifindex;

    /* 3. Send data to the index. */
    res = sendto(fd,
                 HEADER DATA,
                 strlen(HEADER DATA),
                 0,
                 (struct sockaddr *)&socket_address,
                 sizeof(struct sockaddr_ll));
    if (res < 0)
    {
        printf("Cannot send: -%d\n", errno);
        close(fd);
        exit(EXIT_FAILURE);
    }

    return close(fd);
}