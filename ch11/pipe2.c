//
// Created by DemonLee on 10/22/22.
//
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define BUF_SIZ 100

int main() {
    const char *msg = "hello, C 语言";
    const char *response = "okay, 加油";
    char buf[BUF_SIZ];
    bzero(buf, sizeof(buf));

    int fds[2];
    pipe(fds);

    pid_t pid = fork();
    if (0 == pid) {
        printf("child write data to pipe[1]\n");
        write(fds[1], msg, strlen(msg));
        sleep(2);
        read(fds[0], buf, BUF_SIZ);
        printf("child read data from pipe[0]: %s\n", buf);
    } else {
        read(fds[0], buf, BUF_SIZ);
        printf("parent read data from pipe[0]: %s\n", buf);
        write(fds[1], response, strlen(response));
        sleep(3);
    }

    return 0;
}
