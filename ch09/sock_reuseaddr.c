//
// Created by DemonLee on 10/16/22.
//

#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <mach/boolean.h>
#include "common/log.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        error_handling("Usage: sock_reuseaddr <Port> <ReuseAddrFlag>");
    }
    struct sockaddr_in client_addr, server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        error_handling("create socket error");
    }

    int reuse = atoi(argv[2]);
    if (reuse) {
        int option = TRUE;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    }

    if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        error_handling("bind error");
    }

    if (listen(listen_fd, 1024) == -1) {
        error_handling("listen error");
    }

    socklen_t client_len;
    client_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);

    char msg[512];
    ssize_t read_len;
    for (;;) {
        bzero(msg, sizeof(msg));
        read_len = read(client_fd, msg, sizeof(msg));
        if (read_len == 0) {
            printf("client socket closed.\n");
            break;
        } else if (read_len < 0) {
            error_handling("read client msg failed");
        }
        msg[read_len] = '\0';
        printf("%s", msg);
    }
    close(client_fd);
    close(listen_fd);

    return 0;
}
