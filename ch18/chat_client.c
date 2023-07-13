//
// Created by demon on 7/12/23.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

#define NAME_SIZE 20
#define BUFF_SIZE 100

void *send_msg(void *arg);
void *recv_msg(void *arg);

char name[NAME_SIZE] = "[DEFAULT]";

void error_handle(char *msg, int sock) {
    int errnum = errno;
    printf("%s: [%s]\n", msg, strerror(errnum));
    close(sock);
    exit(errnum);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <IP> <Port> <Name>\n", argv[0]);
        return -1;
    }
    if (strlen(argv[3]) >= NAME_SIZE) {
        printf("name is too long\n");
        return -2;
    }

    sprintf(name, "%s", argv[3]);
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("create socket failed: [%s]\n", strerror(sock));
        return -3;
    }

    struct sockaddr_in server_socket;
    memset(&server_socket, '\0', sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = inet_addr(argv[1]);
    server_socket.sin_port = htons(atoi(argv[2]));

    int ret = connect(sock, (struct sockaddr *) &server_socket, sizeof(server_socket));
    if (ret < 0) {
        error_handle("connect server failed", sock);
    }

    pthread_t send_t, recv_t;
    ret = pthread_create(&send_t, NULL, send_msg, (void *) &sock);
    if (ret < 0) {
        error_handle("create send thread failed", sock);
    }
    ret = pthread_create(&recv_t, NULL, recv_msg, (void *) &sock);
    if (ret < 0) {
        error_handle("create recv thread failed", sock);
    }

    void *ret_msg;
    ret = pthread_join(send_t, &ret_msg);
    if (ret < 0) {
        error_handle("join send thread failed", sock);
    }
    printf("send thread exit...\n");
    ret = pthread_join(recv_t, &ret_msg);
    if (ret < 0) {
        error_handle("join recv thread failed", sock);
    }
    printf("recv thread exit...\n");

    close(sock);

    return 0;
}

void *send_msg(void *arg) {
    char *input_ptr = NULL;
    ssize_t write_len;
    int sock = *((int *) arg);
    char name_msg[NAME_SIZE + BUFF_SIZE];
    char msg[BUFF_SIZE];
    bzero(name_msg, sizeof(name_msg));
    bzero(msg, sizeof(msg));
    while (1) {
        input_ptr = fgets(msg, BUFF_SIZE, stdin);
        if (NULL == input_ptr) {
            printf("no data read from stdin...\n");
            break;
        } else if (strcmp(msg, "q\n") == 0 || strcmp(msg, "Q\n") == 0) {
            printf("quit now...\n");
            break;
        }
        sprintf(name_msg, "[%s]: %s", name, msg);
        write_len = write(sock, name_msg, strlen(name_msg));
        if (write_len < 0) {
            printf("send msg failed: [%s]...\n", strerror(errno));
            break;
        }
    }

    close(sock);

    return NULL;
}

void *recv_msg(void *arg) {
    int sock = *((int *) arg);
    ssize_t read_len;
    char name_msg[NAME_SIZE + BUFF_SIZE];
    while (1) {
        read_len = read(sock, name_msg, sizeof(name_msg) - 1);
        if (read_len < 0) {
            printf("read data failed: [%s]...\n", strerror(errno));
            return (void *) -1;
        }
        name_msg[read_len] = '\0';
        fputs(name_msg, stdout);
        fflush(stdout);
    }

    return NULL;
}