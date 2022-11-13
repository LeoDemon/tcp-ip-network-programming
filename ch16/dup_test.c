//
// Created by DemonLee on 11/13/22.
//

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// exercise checking:
// 若文件描述行为读模式，则基于该描述符生成的FILE结构体指针同样是读模式；
// 若文件描达符为写模式，则基于该描述符生成的FILE结构体指针同样是写模式。

void test_read_mode(char *file_name, int open_flag) {
    int read_fd = open(file_name, open_flag);
    if (read_fd == -1) {
        printf("open file error: %s\n", strerror(errno));
    }
    FILE *fp = fdopen(read_fd, "w");
    if (NULL == fp) {
        printf("fdopen fp `w` error: %s\n", strerror(errno));
    } else {
        printf("fdopen fp `w` success...\n");
    }
    fp = fdopen(read_fd, "r");
    if (NULL == fp) {
        printf("fdopen fp `r` error: %s\n", strerror(errno));
    } else {
        printf("fdopen fp `r` success...\n");
    }
    close(read_fd);
}

int main(int argc, char *argv[]) {
    test_read_mode("dup_t1.dat", O_RDONLY | O_CREAT);
    printf("------------------\n");
    test_read_mode("dup_t2.dat", O_WRONLY | O_CREAT);
    return 0;
}
