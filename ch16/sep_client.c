//
// Created by DemonLee on 6/3/22.
//
#include <stdio.h>
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

    FILE *readfp = fdopen(client_socket, "r");
    FILE *writefp = fdopen(client_socket, "w");

    char message[BUF_SIZE];
    while (1) {
        if (fgets(message, BUF_SIZE, readfp) == NULL) {
            break;
        }
        fputs(message, stdout);
        fflush(stdout);
    }
    fputs("From client: Thank you! \n", writefp);
    fflush(writefp);

    fclose(writefp);
    fclose(readfp);

    return 0;
}
