//
// Created by DemonLee on 11/5/22.
//
#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char *argv[]) {
    struct iovec vec[2] = {{"ABCDEFG", 3},
                           {"1234567", 4}};

    // write data to stdout
    ssize_t str_len = writev(1, vec, 2);
    puts("");
    printf("write bytes: %lu\n", str_len);
    return 0;
}
