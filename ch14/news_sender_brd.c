//
// Created by DemonLee on 11/8/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include "common/log.h"

#define BUF_SIZE 30
#define NEWS_FILE_PATH "./news.txt"

void socket_broadcast(int sock_fd);

void send_data(int sock_fd, struct sockaddr_in *addr, socklen_t addr_len) {
    FILE *fp = fopen(NEWS_FILE_PATH, "r");
    if (NULL == fp) {
        error_handling("open news file failed");
    }

    ssize_t send_len;
    char buf[BUF_SIZE];
    while (!feof(fp)) {
        fgets(buf, BUF_SIZE, fp);
        send_len = sendto(sock_fd, buf, strlen(buf), 0, (struct sockaddr *) addr, addr_len);
        if (send_len < 0) {
            printf("send message: [%s] failed: %s", buf, strerror(errno));
            break;
        }
        printf("send buffer: %lu\n", send_len);
        sleep(1);
    }
    fclose(fp);
}

// ./bin/news_sender_brd 255.255.255.255 9899  本地广播
// ./bin/news_sender_brd 192.168.31.255 9899  直接广播
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <BroadcastIP> <port>\n", argv[0]);
        exit(1);
    }

    int sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        error_handling("create socket error");
    }
    // 默认的 Socket 会阻止广播包，故需要更改设置
    socket_broadcast(sock_fd);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    send_data(sock_fd, &addr, sizeof(addr));
    close(sock_fd);

    return 0;
}

// SO_BROADCAST 选项对应的 level 是：SOL_SOCKET
void socket_broadcast(int sock_fd) {
    int broadcast;
    socklen_t broadcast_len = sizeof(broadcast);
    int set_ret = getsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, (void *) &broadcast, &broadcast_len);
    if (set_ret < 0) {
        error_handling("get socket option: SO_BROADCAST failed");
    }
    printf("default broadcast: %d\n", broadcast);

    broadcast = 1;
    set_ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, (void *) &broadcast, sizeof(broadcast));
    if (set_ret < 0) {
        error_handling("set socket option: SO_BROADCAST failed");
    }
}
