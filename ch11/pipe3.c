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

    int fds1[2], fds2[2];
    pipe(fds1), pipe(fds2);

    pid_t pid = fork();
    if (0 == pid) {
        printf("child write data to pipe1[1]\n");
        write(fds1[1], msg, strlen(msg));
        read(fds2[0], buf, BUF_SIZ);
        printf("child read data from pipe2[0]: %s\n", buf);
    } else {
        read(fds1[0], buf, BUF_SIZ);
        printf("parent read data from pipe1[0]: %s\n", buf);
        write(fds2[1], response, strlen(response));
    }

    return 0;
}
