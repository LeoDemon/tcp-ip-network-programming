//
// Created by DemonLee on 5/19/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "log.h"
#include "general.h"

void error_handling(char *message) {
    int error_no = errno;
    fputs(message, stderr);
    if (error_no != 0) {
        fputs(": ", stderr);
        fputs(strerror(error_no), stderr);
    } else {
        error_no = UNKNOWN_ERR_NO;
    }
    fputc('\n', stderr);
    exit(error_no);
}
