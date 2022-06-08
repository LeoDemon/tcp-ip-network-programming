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

    char msg[BUFSIZ];
    size_t msg_len;
    ssize_t msg_send;

    while (1) {
        bzero(msg, sizeof(msg));

        fputs("Input message(q to quit): ", stdout);
        fgets(msg, BUFSIZ, stdin);
        if (!strcmp("q\n", msg) || !strcmp("Q\n", msg)) {
            fputs("quit...\n", stdout);
            break;
        }

        msg_len = strlen(msg);
        if ('\n' == msg[msg_len - 1]) {
            msg[msg_len - 1] = '\0';
        }

        msg_send = sendto(client_socket, msg, msg_len - 1, 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (msg_send != (msg_len - 1)) {
            printf("sendto server failed: [%s]\n", strerror(errno));
            break;
        }
        msg_len = recvfrom(client_socket, msg, BUFSIZ, 0, NULL, NULL);
        if (-1 == msg_len) {
            printf("recvfrom server failed: [%s]\n", strerror(errno));
            break;
        }
        msg[msg_len] = 0;
        printf("server echo: [%s]\n", msg);
    }
    close(client_socket);
    return 0;
}