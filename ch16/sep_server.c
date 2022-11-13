//
// Created by DemonLee on 6/3/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
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
    int client_socket;
    char message[BUF_SIZE];
    FILE *readfp, *writefp;

    for (int i = 0; i < CONNECTION_SIZE; ++i) {
        memset(&client_addr, '\0', sizeof(client_addr));
        printf("waiting connection to accept...\n");
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
        if (-1 == client_socket) {
            error_handling("accept socket failed");
        } else {
            printf("Connected client[%d]: %s:%d\n", i, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }

        readfp = fdopen(client_socket, "r");
        writefp = fdopen(client_socket, "w");

        fputs("From Server: Hi~ Client? \n", writefp);
        fputs("I love all of the world \n", writefp);
        fputs("You are awesome! \n", writefp);
        fflush(writefp);

        // fclose will send `FIN` packet to client, and close the connection.
        fclose(writefp);
        fgets(message, sizeof(message), readfp);
        if (ferror(readfp)) {
            // Bad file descriptor
            printf("fgets from readfp error: %s\n", strerror(errno));
        } else {
            fputs(message, stdout);
        }

        fclose(readfp);
    }

    close(server_socket);

    return 0;
}
