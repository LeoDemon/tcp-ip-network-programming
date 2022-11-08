//
// Created by DemonLee on 11/8/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "common/log.h"

#define BUF_SIZE 30

void recv_data(int socket_fd) {
    char buf[BUF_SIZE];
    ssize_t str_len;

    while (1) {
        str_len = recvfrom(socket_fd, buf, sizeof(buf) - 1, 0, NULL, 0);
        if (str_len < 0) {
            break;
        }
        buf[str_len] = '\0';
        fputs(buf, stdout);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (recv_sock < 0) {
        error_handling("create socket error");
    }

    struct sockaddr_in recv_addr;
    bzero(&recv_addr, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[1]));

    int ret_val = bind(recv_sock, (struct sockaddr *) &recv_addr, sizeof(recv_addr));
    if (ret_val < 0) {
        error_handling("socket addr to socket error");
    }

    recv_data(recv_sock);

    close(recv_sock);

    return 0;
}
