//
// Created by DemonLee on 7/3/22.
//

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <common/log.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 128

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <port> <file_full_path>\n", argv[0]);
        exit(1);
    }

    char *fileFullPath = argv[2];
    FILE *fp = fopen(fileFullPath, "rb");
    if (NULL == fp) {
        char msg[64];
        bzero(&msg, sizeof(msg));
        sprintf(msg, "open file %s failed", fileFullPath);
        error_handling(msg);
    }

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(atoi(argv[1]));

    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        error_handling("create socket failed");
    }

    int iRet = bind(listen_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
    if (-1 == iRet) {
        error_handling("bind socket failed");
    }

    iRet = listen(listen_fd, 5);
    if (-1 == iRet) {
        error_handling("listen socket failed");
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (-1 == client_fd) {
        error_handling("accept connection failed");
    }

    char buf[BUF_SIZE + 1];
    int read_cnt = 0;
    while (1) {
        bzero(buf, sizeof(buf));
        read_cnt = fread(buf, 1, BUF_SIZE, fp);
        if (read_cnt < BUF_SIZE) {
            write(client_fd, buf, read_cnt);
            break;
        }
        write(client_fd, buf, BUF_SIZE);
    }

    printf("shutdown client_socket WR now...\n");
    iRet = shutdown(client_fd, SHUT_WR);
    if (-1 == iRet) {
        error_handling("shutdown client socket failed");
    }

    while (1) {
        bzero(buf, sizeof(buf));
        read_cnt = read(client_fd, buf, BUF_SIZE);
        if (-1 == read_cnt) {
            printf("read data from client error: %s", strerror(errno));
            break;
        } else if (0 == read_cnt) {
            printf("client closed...\n");
            break;
        }
        printf("msg from client: [%s]\n", buf);
    }

    printf("close socket now...\n");

    fclose(fp);
    close(client_fd);
    close(listen_fd);

    return 0;
}
