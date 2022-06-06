//
// Created by DemonLee on 6/5/22.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "common/log.h"
#include "common/general.h"

#define OP_PARAM_SIZE 4
#define OP_RESULT_SIZE 4

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == client_socket) {
        error_handling("create socket failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    int connect_ret = connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (-1 == connect_ret) {
        error_handling("connect socket failed");
    } else {
        printf("connect server success...\n");
    }

    // char message[BUF_SIZE];
    int message[BUF_SIZE] = {0};
    char result[16];
    int message_len = 0, read_len = 0, op_num = 0;
    while (1) {
        fputs("op num: ", stdout);
        scanf("%d", &op_num);
        // input op_num=3, but in ascii table, the 3 is 'ETX': end of text.
        // so cannot convert op_num directly, like this:
        // char ch_num = (char)op_num;
        // we should convert int to char like this:
//        char ch_num = op_num + '0'; // '0' ascii code is: 48
//        printf("op_num.int=[%d], char=[%c]\n", op_num, ch_num);
//        message[0] = ch_num;

        message[0] = op_num;
        for (int i = 1; i <= op_num; ++i) {
            printf("op_param[%d]: ", i);
            scanf("%d", &message[i]);
        }

        // remove the '\n' char in the last stdin
        char last_char = fgetc(stdin);
        printf("last_char=[%c]\n", last_char);

        char op = '\0';
        printf("operator: ", stdout);
        scanf("%c", &op);
        message[op_num + 1] = op;

        // message_len = strlen(message);
        message_len = (op_num + 2) * OP_PARAM_SIZE;
        int write_len = write(client_socket, message, message_len);
        if (message_len != write_len) {
            printf("write message error, quit...\n");
            break;
        }
        printf("send message len: [%d]\n", write_len);

        bzero(result, sizeof(result));
        read_len = read(client_socket, result, 10);
        if (-1 == read_len) {
            error_handling("read from server failed");
        } else if (0 == read_len) {
            error_handling("server closed");
        }

        result[OP_RESULT_SIZE] = '\0';
        printf("Calculate result: %s\n", result);
    }
    close(client_socket);

    return 0;
}
