//
// Created by DemonLee on 10/23/22.
//

#include <stdio.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    fd_set reads, temps;
    FD_ZERO(&reads);
    FD_SET(0, &reads); // stdin

    char buf[BUFSIZ];
    ssize_t read_len;
    int result;
    struct timeval timeout;
    // macOS 12.6（Arm64）下将超时放在此处没有问题，即不需要放在下面的 while 循环中
    // timeout.tv_sec = 3;
    // timeout.tv_usec = 0;
    while (1) {
        // Linux 下需要放这里
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        printf("---timeout: %ld\n", timeout.tv_sec);
        printf("---reads[0]: %ld, temps[0]: %ld\n", reads.__fds_bits[0], temps.__fds_bits[0]);
        temps = reads;
        result = select(1, &temps, NULL, NULL, &timeout);
        if (-1 == result) {
            printf("select() error: %s\n", strerror(errno));
            break;
        }
        if (0 == result) {
            printf("select() timeout\n");
            continue;
        }
        if (FD_ISSET(0, &temps)) {
            read_len = read(0, buf, BUFSIZ);
            if (read_len < 0) {
                printf("read error from stdin: %s\n", strerror(errno));
                continue;
            }
            buf[read_len] = '\0';
            printf("message from stdin: %s", buf);
            if (!strcmp("Q\n", buf) || !strcmp("q\n", buf)) {
                printf("Bye bye...\n");
                break;
            }
        }
    }

    return 0;
}
