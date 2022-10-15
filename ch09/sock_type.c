//
// Created by DemonLee on 10/15/22.
//
#include <stdio.h>
#include <sys/socket.h>
#include "common/log.h"

void check_socket_type(int socket_type_expect, int socket_fd);

int main(int argc, char *argv[]) {
    int tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0) {
        error_handling("create tcp socket failed");
    }
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        error_handling("create udp socket failed");
    }

    check_socket_type(SOCK_STREAM, tcp_socket_fd);
    check_socket_type(SOCK_DGRAM, udp_socket_fd);

    return 0;
}

void check_socket_type(int socket_type_expect, int socket_fd) {
    int socket_type;
    socklen_t socket_type_len = sizeof(socket_type);

    int state = getsockopt(socket_fd, SOL_SOCKET, SO_TYPE, &socket_type, &socket_type_len);
    if (state < 0) {
        error_handling("get socket type failed");
    }
    if (socket_type != socket_type_expect) {
        printf("get socket type incorrect: %d, expected: %d\n", socket_type, socket_type_expect);
    }
}
