//
// Created by DemonLee on 7/3/22.
//
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <common/log.h>
#include "hostent.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <domain>\n", argv[0]);
        exit(2);
    }

    struct hostent *host = gethostbyname(argv[1]);
    if (host == NULL){
        error_handling("gethostbyname failed");
    }
    display(host);

    return 0;
}