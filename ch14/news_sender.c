//
// Created by DemonLee on 11/7/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include "common/log.h"

#define TTL 64
#define BUF_SIZE 30
#define NEWS_FILE_PATH "./news.txt"

void send_data(int sock_fd, struct sockaddr_in *multi_addr, socklen_t multi_addr_len) {
    FILE *fp = fopen(NEWS_FILE_PATH, "r");
    if (NULL == fp) {
        error_handling("open news file failed");
    }

    ssize_t send_len;
    char buf[BUF_SIZE];
    while (!feof(fp)) {
        fgets(buf, BUF_SIZE, fp);
        send_len = sendto(sock_fd, buf, strlen(buf), 0, (struct sockaddr *) multi_addr, multi_addr_len);
        if (send_len < 0) {
            printf("send message: [%s] failed: %s", buf, strerror(errno));
            break;
        }
        printf("send buffer: %lu\n", send_len);
        sleep(1);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <GroupIP> <port>\n", argv[0]);
        exit(1);
    }

    int sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        error_handling("create socket error");
    }
    // 为了传递多播数据包，必须设置 TTL
    int time_live = TTL;
    int set_ret = setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &time_live, sizeof(time_live));
    if (set_ret < 0) {
        error_handling("set socket option: IP_MULTICAST_TTL failed");
    }

    struct sockaddr_in multi_addr;
    bzero(&multi_addr, sizeof(multi_addr));
    multi_addr.sin_family = AF_INET;
    multi_addr.sin_port = htons(atoi(argv[2]));
    multi_addr.sin_addr.s_addr = inet_addr(argv[1]);

    send_data(sock_fd, &multi_addr, sizeof(multi_addr));
    close(sock_fd);

    return 0;
}
