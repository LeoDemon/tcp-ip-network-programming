//
// Created by DemonLee on 6/5/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "common/log.h"
#include "common/general.h"

#define CONNECTION_SIZE 5
#define OP_PARAM_SIZE 4

// step1: define data input format: opt_cnt~opt_params~opt_symbol
// step2: define data output format: if error, return error msg, else return calculate result
// step3: define calculate function
// step4: check input args: +,-,* and numbers
// step5: convert socket input data to calculate input args
// step6: test

void calculate(int op_num, int op_params[], char op, char *result) {
    if (op_num <= 0) {
        printf("op_num=[%d] is invalid\n", op_num);
        sprintf(result, "%s", "err");
        return;
    }
    if ('+' != op && '-' != op && '*' != op) {
        printf("op=[%c] is invalid\n", op);
        sprintf(result, "%s", "err");
        return;
    }

    int i = 1;
    int ret = op_params[0];
    switch (op) {
        case '+': {
            for (; i < op_num; ++i) {
                ret += op_params[i];
            }
            break;
        }
        case '-': {
            for (; i < op_num; ++i) {
                ret -= op_params[i];
            }
            break;
        }
        case '*': {
            for (; i < op_num; ++i) {
                ret *= op_params[i];
            }
            break;
        }
        default: {
            printf("invalid op=[%c]\n", op);
            break;
        }
    }

    sprintf(result, "%d", ret);
}

/**
 *
 * @param read_len
 * @param i
 * @return 0: normal, 1: continue, 2: break
 */
int socket_read_check(int read_len, int i) {
    if (read_len <= 0) {
        if (-1 == read_len) {
            printf("read from client[%d] error: [%s]\n", i, strerror(errno));
        } else {
            printf("client[%d] closed\n", i);
        }
        return 2;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == server_socket) {
        error_handling("create socket failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    int bind_ret = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (-1 == bind_ret) {
        error_handling("bind socket failed");
    }

    int listen_ret = listen(server_socket, CONNECTION_SIZE);
    if (-1 == listen_ret) {
        error_handling("listen socket failed");
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket, read_len, op_num, op_recv_len, op_total_len, cal_result_len;
    char message[BUF_SIZE];
    char cal_result[64];

    for (int i = 0; i < CONNECTION_SIZE; ++i) {
        memset(&client_addr, '\0', sizeof(client_addr));
        printf("waiting connection to accept...\n");
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
        if (-1 == client_socket) {
            error_handling("accept socket failed");
        } else {
            printf("Connected client[%d]: %s:%d\n", i, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }

        while (1) {
            bzero(message, sizeof(message));
            read_len = read(client_socket, message, 1);
            int read_len_check_flag = socket_read_check(read_len, i);
            if (2 == read_len_check_flag) {
                close(client_socket);
                break;
            }
            op_num = message[0] - '0';
            printf("op_num=[%d]\n", op_num);
            if (op_num <= 0) {
                printf("invalid op_num=[%d]\n", op_num);
                close(client_socket);
                break;
            }

            // get op_params and op
            op_total_len = OP_PARAM_SIZE * op_num + 1;
            op_recv_len = 0;
            while (op_recv_len < op_total_len) {
                read_len = read(client_socket, &message[op_recv_len + 1], op_total_len);
                op_recv_len += read_len;
            }

            bzero(cal_result, sizeof(cal_result));
            calculate(op_num, (int *) &message[1], message[op_total_len], cal_result);

            cal_result_len = strlen(cal_result);
            if (cal_result_len != write(client_socket, cal_result, cal_result_len)) {
                printf("write to client[%d] failed: [%s]\n", i, strerror(errno));
                close(client_socket);
                break;
            }
        }
    }

    return 0;
}
