//
// Created by DemonLee on 6/8/22.
//

#ifndef TCP_IP_NETWORK_PROGRAMMING_SOCKET_UTIL_H
#define TCP_IP_NETWORK_PROGRAMMING_SOCKET_UTIL_H

#include <arpa/inet.h>
#include <stdio.h>

void parse_socket_addr(struct sockaddr_in sock_addr, char *addr);

#endif //TCP_IP_NETWORK_PROGRAMMING_SOCKET_UTIL_H
