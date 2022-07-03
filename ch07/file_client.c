//
// Created by DemonLee on 7/3/22.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <common/log.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 128
#define FILE_NAME "file_client_recv.txt"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(FILE_NAME, "wb");
    if (NULL == fp) {
        char msg[64];
        bzero(&msg, sizeof(msg));
        sprintf(msg, "open file %s failed", FILE_NAME);
        error_handling(msg);
    }

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    srv_addr.sin_port = htons(atoi(argv[2]));

    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == client_socket) {
        error_handling("create socket failed");
    }

    int iRet = connect(client_socket, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
    if (-1 == iRet) {
        error_handling("connect server failed");
    }

    char buf[BUF_SIZE + 1];
    int read_cnt = 0;
    while (1) {
        bzero(buf, sizeof(buf));
        read_cnt = read(client_socket, buf, BUF_SIZE);
        if (-1 == read_cnt) {
            printf("read data from server error: %s", strerror(errno));
            break;
        } else if (0 == read_cnt) {
            printf("server closed...\n");
            break;
        }
        fwrite(buf, 1, read_cnt, fp);
    }

    printf("received file data\n");
    char *feedback = "Thank you!";
    iRet = write(client_socket, feedback, strlen(feedback));
    if (-1 == iRet) {
        printf("write data to server failed: %s", strerror(errno));
    }

    sleep(3);
    printf("close socket now...");

    fclose(fp);
    close(client_socket);

    return 0;
}