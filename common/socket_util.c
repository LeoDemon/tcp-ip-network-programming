//
// Created by DemonLee on 6/8/22.
//

#include "socket_util.h"

void parse_socket_addr(struct sockaddr_in sock_addr, char *addr) {
    if (NULL == addr) {
        printf("input addr is NULL\n");
        return;
    }

    char *ip = inet_ntoa(sock_addr.sin_addr);
    int port = ntohs(sock_addr.sin_port);
    sprintf(addr, "%s:%d", ip, port);
}
