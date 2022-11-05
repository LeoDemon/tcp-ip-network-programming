//
// Created by DemonLee on 11/5/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include "common/log.h"

void write_handling(ssize_t ret_val, const char *method) {
    if (ret_val < 0) {
        printf("%s data error: %s\n", method, strerror(errno));
    }
}

void send_data(int sock_fd) {
    char *data[] = {"123", "4", "567", "890"};
    printf("write: %s\n", data[0]);
    ssize_t ret_val = write(sock_fd, data[0], strlen(data[0]));
    write_handling(ret_val, "write");

    printf("send: %s\n", data[1]);
    ret_val = send(sock_fd, data[1], strlen(data[1]), MSG_OOB);
    write_handling(ret_val, "send");

    printf("write: %s\n", data[2]);
    ret_val = write(sock_fd, data[2], strlen(data[2]));
    write_handling(ret_val, "write");

    sleep(1);
    printf("send: %s\n", data[3]);
    ret_val = send(sock_fd, data[3], strlen(data[3]), MSG_OOB);
    write_handling(ret_val, "send");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        error_handling("create socket error");
    }

    struct sockaddr_in recv_addr;
    bzero(&recv_addr, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(atoi(argv[2]));
    recv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    int ret_val = connect(sock_fd, (struct sockaddr *) &recv_addr, sizeof(recv_addr));
    if (ret_val < 0) {
        error_handling("connect to server error");
    }

    send_data(sock_fd);
    sleep(1);
    close(sock_fd);

    return 0;
}
