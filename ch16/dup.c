//
// Created by DemonLee on 11/13/22.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// copy file descriptor
int main(int argc, char *argv[]) {
    char str1[] = "Hi~ \n";
    char str2[] = "It's a nice day~ \n";

    int cfd1 = dup(1);
    int cfd2 = dup2(cfd1, 10);

    printf("fd1=%d, fd2=%d\n", cfd1, cfd2);
    write(cfd1, str1, strlen(str1));
    write(cfd2, str2, strlen(str2));

    close(cfd1);
    close(cfd2);

    write(1, str1, strlen(str1));
    close(1);

    ssize_t write_len = write(1, str2, strlen(str2));
    if (write_len < 0) {
        // we can use debug mode to check the errno and errmsg.
        int err_num = errno;
        char *errmsg = strerror(err_num);
        // stdout has been closed, so no output here.
        printf("write data to stdout failed: %s\n", errmsg);
    }

    return 0;
}
