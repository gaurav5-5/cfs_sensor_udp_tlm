#include "main.h"
#include "common_types.h"
#include "logger.h"
#include "udp_listener.h"

#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    char recv_buf[MAX_BUFLEN];

    uint32_t hostname = 0;
    inet_pton(AF_INET, HOSTNAME, &(hostname));

    UDPLogger_t udpl = {.logfile  = DEFAULT_LOGFILE_PATH,
                        .sockfd   = -1,
                        .port     = PORT,
                        .hostname = hostname};

    int retCode = UDPListener_Init(&udpl);
    if (EXIT_FAILURE == retCode) {
        exit(retCode);
    } else {
        (void)printf("Server listening on %s:%d\n", HOSTNAME, PORT);
    }

    while (1) {
        int retCode = UDPListener_Recv(&udpl, recv_buf, MAX_BUFLEN);
        if (0 < retCode) {
            if (31 >= retCode)
                continue;

            uint16_t sensor_id;
            uint16_t pkt_count;
            int32_t  sensor_x, sensor_y, sensor_z;

            sensor_id = ntohs(*(uint16_t *)&recv_buf[15]);

            pkt_count = ntohs(*(uint16_t *)&recv_buf[17]);
            sensor_x  = ntohl(*(int32_t *)&recv_buf[20]);
            sensor_y  = ntohl(*(int32_t *)&recv_buf[24]);
            sensor_z  = ntohl(*(int32_t *)&recv_buf[28]);

            Logger_printf(&udpl, "ID: %u | CNT: %u | X: %d | Y: %d | Z: %d",
                          sensor_id, pkt_count, sensor_x, sensor_y, sensor_z);
        }
    }

    UDPListener_Close(&udpl);

    return EXIT_SUCCESS;
}
