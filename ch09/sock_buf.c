//
// Created by DemonLee on 10/15/22.
//
#include <stdio.h>
#include <sys/socket.h>
#include "common/log.h"

void get_socket_buf(const char *socket_type, int socket_fd);

void set_socket_buf(const char *socket_type, int socket_fd, int rcv_size, int send_size);

void test_tcp_socket_buf();

void test_udp_socket_buf();

int main(int argc, char *argv[]) {
    test_tcp_socket_buf();
    fprintf(stdout, "--------------------\n");
    test_udp_socket_buf();

    return 0;
}

void test_tcp_socket_buf() {
    int tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0) {
        error_handling("create tcp socket failed");
    }
    get_socket_buf("tcp", tcp_socket_fd);
    set_socket_buf("tcp", tcp_socket_fd, 4096, 4096);
    get_socket_buf("tcp", tcp_socket_fd);
    set_socket_buf("tcp", tcp_socket_fd, 409600, 409600);
    get_socket_buf("tcp", tcp_socket_fd);
    set_socket_buf("tcp", tcp_socket_fd, 40960000, 40960000);
    get_socket_buf("tcp", tcp_socket_fd);
}

void test_udp_socket_buf() {
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        error_handling("create udp socket failed");
    }
    get_socket_buf("udp", udp_socket_fd);
    set_socket_buf("udp", udp_socket_fd, 4096, 4096);
    get_socket_buf("udp", udp_socket_fd);
    set_socket_buf("udp", udp_socket_fd, 409600, 409600);
    get_socket_buf("udp", udp_socket_fd);
    set_socket_buf("udp", udp_socket_fd, 40960000, 40960000);
    get_socket_buf("udp", udp_socket_fd);
}

void get_socket_buf(const char *socket_type, int socket_fd) {
    int buf;
    socklen_t buf_len = sizeof(buf);

    int state = getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &buf, &buf_len);
    if (state < 0) {
        error_handling("get socket receive buffer failed");
    }
    fprintf(stdout, "%s receive buf: %d\n", socket_type, buf);

    state = getsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &buf, &buf_len);
    if (state < 0) {
        error_handling("get socket send buffer failed");
    }
    fprintf(stdout, "%s send buf: %d\n", socket_type, buf);
    fflush(stdout);
}

void set_socket_buf(const char *socket_type, int socket_fd, int rcv_size, int send_size) {
    int state = setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, sizeof(rcv_size));
    if (state < 0) {
        fprintf(stderr, "set %s socket receive buffer: %d failed\n", socket_type, rcv_size);
    }

    state = setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &send_size, sizeof(send_size));
    if (state < 0) {
        fprintf(stderr, "set %s socket send buffer: %d failed\n", socket_type, send_size);
    }
}
