//
// Created by DemonLee on 11/5/22.
//

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "common/log.h"

#define BUF_SIZE 30

void recv_data(int socket_fd) {
    char buf[BUF_SIZE];
    ssize_t str_len;

    while (1) {
        str_len = recv(socket_fd, buf, sizeof(buf) - 1, MSG_PEEK | MSG_DONTWAIT);
        if (str_len == 0) {
            printf("client close connection...\n");
            break;
        } else if (str_len < 0) {
            // printf("recv data error via MSG_PEEK|MSG_DONTWAIT: %s\n", strerror(errno));
            sleep(1);
            continue;
        }
        bzero(buf, sizeof(buf));
        printf("check data in buffer: [%s]\n", buf);

        bzero(buf, sizeof(buf));
        str_len = recv(socket_fd, buf, sizeof(buf) - 1, 0);
        if (str_len == -1) {
            printf("recv data error: %s\n", strerror(errno));
            continue;
        }
        printf("read data from buffer again: [%s]\n", buf);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (acpt_sock < 0) {
        error_handling("create socket error");
    }

    struct sockaddr_in recv_addr;
    bzero(&recv_addr, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[1]));

    int ret_val = bind(acpt_sock, (struct sockaddr *) &recv_addr, sizeof(recv_addr));
    if (ret_val < 0) {
        error_handling("socket addr to socket error");
    }

    ret_val = listen(acpt_sock, 1024);
    if (ret_val < 0) {
        error_handling("listen for connections on a socket error");
    }

    struct sockaddr_in send_addr;
    socklen_t send_addr_len = sizeof(send_addr);
    int client_sock = accept(acpt_sock, (struct sockaddr *) &send_addr, &send_addr_len);
    if (client_sock < 0) {
        error_handling("accept a connection on a socket error");
    }

    ret_val = fcntl(client_sock, F_SETOWN, getpid());
    if (ret_val < 0) {
        error_handling("fcntl SIGURG error");
    }
    recv_data(client_sock);

    close(client_sock);
    close(acpt_sock);

    return 0;
}
