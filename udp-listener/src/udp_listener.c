#include "udp_listener.h"
#include "common_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int UDPListener_Init(UDPLogger_t *udpl) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;

    if (fd < 0) {
        perror("Socket Creation Failed\n");
        return EXIT_FAILURE;
    }

    udpl->sockfd = fd;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = udpl->hostname;
    server_addr.sin_port        = htons(udpl->port);

    int retCode = bind(udpl->sockfd, (const struct sockaddr *)&server_addr,
                       sizeof(server_addr));
    if (retCode < 0) {
        perror("Socket Bind Error\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int UDPListener_Recv(UDPLogger_t *udpl, char *buf, size_t len) {
    ssize_t msgLen = recvfrom(udpl->sockfd, buf, len, 0, NULL, NULL);
    return msgLen;
}

int UDPListener_Close(UDPLogger_t *udpl) {
    if (0 <= udpl->sockfd) {
        int retCode = close(udpl->sockfd);
        if (0 == retCode) {
            perror("Failure in Closing Socket\n");
        }
        return retCode;
    }

    return 0;
}
