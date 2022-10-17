//
// Created by DemonLee on 10/17/22.
//
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "common/log.h"

void get_tcp_nodelay(int socket_fd);

int main(int argc, char *argv[]) {
    int tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0) {
        error_handling("create tcp socket failed");
    }

    get_tcp_nodelay(tcp_socket_fd);

    int off = 1;
    int state = setsockopt(tcp_socket_fd, IPPROTO_TCP, TCP_NODELAY, &off, sizeof(off));
    if (state < 0) {
        error_handling("set TCP_NODELAY off failed");
    }

    get_tcp_nodelay(tcp_socket_fd);

    return 0;
}

void get_tcp_nodelay(int socket_fd) {
    int nagle_flag;
    socklen_t len = sizeof(nagle_flag);

    int state = getsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &nagle_flag, &len);
    if (state < 0) {
        error_handling("get TCP_NODELAY flag failed");
    }
    printf("get TCP_NODELAY: %d\n", nagle_flag);
}
