//
// Created by DemonLee on 6/3/22.
//
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "common/log.h"
#include "common/general.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == client_socket) {
        error_handling("create socket failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    int connect_ret = connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (-1 == connect_ret) {
        error_handling("connect socket failed");
    } else {
        printf("connect server success...\n");
    }

    char message[BUF_SIZE];
    int message_len;
    while (1) {
        fputs("Input(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            printf("quit...\n");
            break;
        }

        message_len = strlen(message);
        if (message_len != write(client_socket, message, strlen(message))) {
            printf("write message error, quit...\n");
            break;
        }

        message_len = read(client_socket, message, BUF_SIZE);
        if (-1 == message_len) {
            printf("read from server error: [%s]\n", strerror(errno));
            break;
        } else if (0 == message_len) {
            printf("server closed\n");
            break;
        }

        message[message_len] = '\0';
        printf("Message from server: %s", message);
    }
    close(client_socket);

    return 0;
}