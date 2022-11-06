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

#define BUF_SIZE 30

void write_handling(ssize_t ret_val, const char *method) {
    if (ret_val < 0) {
        printf("%s data error: %s\n", method, strerror(errno));
    }
}

void send_data(int sock_fd) {
    char *input_ptr = NULL;
    char buf[BUF_SIZE];
    while (1) {
        bzero(buf, sizeof(buf));
        input_ptr = fgets(buf, sizeof(buf), stdin);
        if (NULL == input_ptr) {
            break;
        }
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = '\0';
        }
        printf("input_ptr: [%s], buf: [%s]\n", input_ptr, buf);
        if (strcmp("q", buf) == 0 || strcmp("Q", buf) == 0) {
            break;
        }
        ssize_t ret_val = write(sock_fd, buf, strlen(buf));
        write_handling(ret_val, "write");
    }
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
    close(sock_fd);

    return 0;
}
