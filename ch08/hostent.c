//
// Created by DemonLee on 7/3/22.
//

#include "hostent.h"

void display(const struct hostent *host) {
    printf("Official name: %s\n", host->h_name);
    printf("Address type: %d: %s\n", host->h_addrtype, host->h_addrtype==AF_INET?"AF_INET":"AF_INET6");
    printf("Address len: %d\n", host->h_length);

    int i = 0;
    for(i=0;host->h_aliases[i];i++) {
        printf("Aliases[%d]: %s\n", i, host->h_aliases[i]);
    }
    for(i=0;host->h_addr_list[i];i++) {
        struct in_addr *inAddr = (struct in_addr*) host->h_addr_list[i];
        const char *inAddrStr = inet_ntoa(*inAddr);
        printf("addr[%d]: %s\n", i, inAddrStr);
    }
}