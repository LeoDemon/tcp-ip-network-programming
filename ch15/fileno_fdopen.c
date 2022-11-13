//
// Created by DemonLee on 11/13/22.
//

#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd = open("data.dat", O_CREAT | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        fputs("file open error", stdout);
        return -1;
    }

    printf("First file descriptor: %d\n", fd);
    // fdopen: file descriptor --> FILE struct pointer
    FILE *fp = fdopen(fd, "w");
    fputs("Canonical正式发布专为物联网和嵌入式设备优化的Ubuntu Core 22操作系统\n", fp);

    // fileno: FILE struct pointer --> file descriptor
    int fd2 = fileno(fp);
    printf("Second file descriptor: %d\n", fd2);

    fclose(fp);

    return 0;
}
