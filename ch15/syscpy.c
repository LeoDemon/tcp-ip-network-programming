//
// Created by DemonLee on 11/13/22.
//

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "common/datetime.h"

#define BUF_SIZE 3

// system io function
int main(int argc, char *argv[]) {
    int fd1 = open("news-1.txt", O_RDONLY);
    int fd2 = open("syscpy.txt", O_WRONLY | O_CREAT | O_TRUNC);

    display_now();

    char buf[BUF_SIZE];
    ssize_t len;
    while ((len = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, len);
    }

    display_now();

    close(fd1);
    close(fd2);

    return 0;
}
