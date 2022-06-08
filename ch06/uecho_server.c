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
#include "common/socket_util.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == server_socket) {
        error_handling("create socket failed");
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_socket));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iret = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (-1 == iret) {
        error_handling("bind socket failed");
    }

    struct sockaddr_in client_socket_addr;
    socklen_t client_addr_len;
    char msg[BUFSIZ];
    char client_addr[32];
    size_t msg_len;

    while (1) {
        bzero(msg, sizeof(msg));
        bzero(client_addr, sizeof(client_addr));

        msg_len = recvfrom(server_socket, msg, BUFSIZ, 0, (struct sockaddr *) &client_socket_addr, &client_addr_len);
        if (-1 == msg_len) {
            printf("recvfrom client failed: [%s]\n", strerror(errno));
            break;
        }

        parse_socket_addr(client_socket_addr, client_addr);
        printf("client[%s]: [%s]\n", client_addr, msg);

        msg[msg_len] = 0;
        if (msg_len !=
            sendto(server_socket, msg, msg_len, 0, (struct sockaddr *) &client_socket_addr, client_addr_len)) {
            printf("sendto client failed: [%s]\n", strerror(errno));
            continue;
        }
    }

    close(server_socket);
    return 0;
}