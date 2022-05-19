//
// Created by DemonLee on 5/19/22.
//

#include <stdio.h>
#include <stdlib.h>

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(2);
}
