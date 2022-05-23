//
// Created by DemonLee on 5/19/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../common/log.h"

int main(int argc, char *argv[]) {

    if (3 != argc) {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        error_handling("socket() error");
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (-1 == connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        error_handling("connect() error");
    }

    char message[1024];
    int str_len = read(sock, message, sizeof(message) - 1);
    if (-1 == str_len) {
        error_handling("read() error");
    }

    printf("Message from server: %s \n", message);

    close(sock);

    return 0;
}
