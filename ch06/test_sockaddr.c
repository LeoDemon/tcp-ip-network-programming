//
// Created by DemonLee on 5/24/22.
//

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = PF_INET;

    // inet_addr() will convert byte order to Network Order automatic
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.22");
    in_addr_t addr = htonl(inet_addr("192.168.1.22"));
    server_addr.sin_port = htons(0x1234);

    printf("s_addr=[%#lx], addr=[%#lx]\n", server_addr.sin_addr.s_addr, addr);

    struct sockaddr *sockaddr = (struct sockaddr*) &server_addr;

    printf("sockaddr.len: [%d]\n", sockaddr->sa_len);
    printf("sockaddr.family: [%d]\n", sockaddr->sa_family);
    printf("sockaddr.data: [%s]\n", sockaddr->sa_data);
    for(int i=0;i<14;i++){
        printf("sockaddr.data[%d]: [%c]\n", i, sockaddr->sa_data[i]);
    }

    return 0;
}
