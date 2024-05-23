#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

int main(int argc, char **argv)
{
    int res = 0;
    int fd = socket(AF_PACKET, SOCK_RAW, 0);
    if (fd < 0)
    {
        printf("Cannot create socket: -%d\n", errno);
        exit(EXIT_FAILURE);
    }

    res = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, "vni0", strlen("vni0") + 1);
    if (res < 0)
    {
        printf("Cannot setsockopt: -%d\n", errno);
        close(fd);
        exit(EXIT_FAILURE);
    }

    res = send(fd, "Hi there", strlen("Hi There"), 0);
    if (res < 0)
    {
        printf("Cannot send: -%d\n", errno);
        close(fd);
        exit(EXIT_FAILURE);
    }

    return close(fd);
}