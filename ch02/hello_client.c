//
// Created by DemonLee on 5/19/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error_handling(char *message);

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

    char message[10240];
    // char *msg = message;
    int str_len = 0;
    int read_len = 0;
    //int i = 0;
    int read_count = 0;
    sleep(3);
    while (1) {
        read_count++;
        //str_len = read(sock, &message[i++], 1);
        //printf("Message from server: [%s], reading count: [%d] \n", message, i);
        read_len = read(sock, &message[str_len], 10240);
        if (-1 == read_len) {
            error_handling("read() error");
            break;
        }
        if (0 == read_len) {
            printf("reading end...\n");
            break;
        }
        str_len += read_len;
        printf("Message from server: [%s], read_len: [%d], str_len: [%d] \n", message, read_len, str_len);
    }

    printf("Message from server: [%s], reading count: [%d] \n", message, read_count);

    close(sock);

    return 0;
}
