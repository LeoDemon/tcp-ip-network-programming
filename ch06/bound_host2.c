//
// Created by DemonLee on 6/8/22.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "common/log.h"

#define SEND_SIZE 3

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int client_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == client_socket) {
        error_handling("create socket failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    char msg[SEND_SIZE][BUFSIZ];
    bzero(msg, sizeof(msg));
    size_t msg_len;
    ssize_t msg_send;

    for (int i = 0; i < SEND_SIZE; i++) {
        fputs("Input message: ", stdout);
        fgets(msg[i], BUFSIZ, stdin);

        msg_len = strlen(msg[i]);
        if ('\n' == msg[i][msg_len - 1]) {
            msg[i][msg_len - 1] = '\0';
        }
    }

    for (int i = 0; i < SEND_SIZE; i++) {
        msg_len = strlen(msg[i]);
        msg_send = sendto(client_socket, msg[i], msg_len, 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (msg_send != (msg_len)) {
            printf("sendto server failed: [%s]\n", strerror(errno));
        }
        printf("send msg[%d] success: [%s]\n", i, msg[i]);
    }

    close(client_socket);

    return 0;
}