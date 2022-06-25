//
// Created by DemonLee on 6/3/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "common/log.h"
#include "common/general.h"

#define CONNECTION_SIZE 5

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == server_socket) {
        error_handling("create socket failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    int bind_ret = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (-1 == bind_ret) {
        error_handling("bind socket failed");
    }

    int listen_ret = listen(server_socket, CONNECTION_SIZE);
    if (-1 == listen_ret) {
        error_handling("listen socket failed");
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket, message_len;
    char message[BUF_SIZE];

    for (int i = 0; i < CONNECTION_SIZE; ++i) {
        memset(&client_addr, '\0', sizeof(client_addr));
        printf("waiting connection to accept...\n");
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
        if (-1 == client_socket) {
            error_handling("accept socket failed");
        } else {
            printf("Connected client[%d]: %s:%d\n", i, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }

        while (1) {
            message_len = read(client_socket, message, BUF_SIZE);
            if (message_len <= 0) {
                if (-1 == message_len) {
                    printf("read from client[%d] error: [%s]\n", i, strerror(errno));
                } else {
                    printf("client[%d] closed\n", i);
                }
                close(client_socket);
                break;
            }
            if (message_len != write(client_socket, message, message_len)) {
                printf("write to client[%d] failed: [%s]\n", i, strerror(errno));
            }
        }
    }

    close(server_socket);

    return 0;
}
