//
// Created by DemonLee on 7/3/22.
//
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <common/log.h>
#include <string.h>
#include "hostent.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <IP>\n", argv[0]);
        exit(2);
    }

    struct in_addr ip;
    bzero(&ip, sizeof(ip));
    ip.s_addr = inet_addr(argv[1]);

    struct hostent *host = gethostbyaddr((const void *)&ip, sizeof(ip), AF_INET);
    if (host == NULL){
        error_handling("gethostbyaddr failed");
    }
    display(host);

    return 0;
}
