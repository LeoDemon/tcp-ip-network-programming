//
// Created by DemonLee on 5/19/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error_handling(char *message);

int main(int argc, char *argv[]) {

    if (2 != argc) {
        printf("Usage: %s <port> \n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == serv_sock) {
        error_handling("socket() error");
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (-1 == bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        error_handling("bind() error");
    }

    if (-1 == listen(serv_sock, 5)) {
        error_handling("listen() error");
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_addr_size);
    if (-1 == client_sock) {
        error_handling("accept() error");
    }

    char message[] = "Hello, Client!";
    write(client_sock, message, sizeof(message));
    close(serv_sock);
    close(client_sock);

    return 0;
}
