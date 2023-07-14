//
// Created by demon on 7/12/23.
//
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 100
#define MAX_CLIENT 256

void *handle_client(void *arg);

void send_msg_to_all(char *msg, ssize_t len);

int client_cnt = 0;
int client_socks[MAX_CLIENT];
pthread_mutex_t mutex;

void error_handle(char *msg, int sock) {
    int errnum = errno;
    printf("%s: [%s]\n", msg, strerror(errnum));
    close(sock);
    exit(errnum);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        return -1;
    }
    pthread_mutex_init(&mutex, NULL);

    struct sockaddr_in server_sockaddr, client_sockaddr;
    memset(&server_sockaddr, '\0', sizeof(server_sockaddr));
    socklen_t client_sockaddr_len = sizeof(client_sockaddr);

    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        printf("create socket failed: [%s]\n", strerror(errno));
    }

    int reuse = 1;
    int ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0) {
        error_handle("set socket option failed", server_sock);
    }

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr.sin_port = htons(atoi(argv[1]));

    ret = bind(server_sock, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr));
    if (ret < 0) {
        error_handle("bind socket failed", server_sock);
    }

    ret = listen(server_sock, MAX_CLIENT);
    if (ret < 0) {
        error_handle("listen socket failed", server_sock);
    }

    pthread_t t_id;
    int client_socket;
    while (1) {
        memset(&client_sockaddr, '\0', client_sockaddr_len);
        client_socket = accept(server_sock, (struct sockaddr *) &client_sockaddr, &client_sockaddr_len);
        if (client_socket < 0) {
            printf("accept client socket failed: [%s]\n", strerror(errno));
            continue;
        }

        pthread_mutex_lock(&mutex);
        client_socks[client_cnt++] = client_socket;
        pthread_mutex_unlock(&mutex);

        // Todo: 这里的容错性不足
        ret = pthread_create(&t_id, NULL, handle_client, (void *) &client_socket);
        if (ret < 0) {
            printf("create thread for client-%d failed: %s", client_socket, strerror(errno));
            continue;
        }

        // 这里使用 pthread_detach 来分离并引导线程退出
        // The  pthread_detach()  function  marks the thread identified by thread as detached.
        // When a detached thread terminates, its resources are automatically released back to the system without the
        // need for another thread to join with the terminated thread.
        ret = pthread_detach(t_id);
        if (ret < 0) {
            printf("detach thread for client-%d failed: %s", client_socket, strerror(errno));
        }
        printf("connect client: %s:%d success...\n", inet_ntoa(client_sockaddr.sin_addr),
               ntohs(client_sockaddr.sin_port));
    }

    close(server_sock);

    return 0;
}

void send_msg_to_all(char *msg, ssize_t len) {
    pthread_mutex_lock(&mutex);

    ssize_t write_len;
    for (int i = 0; i < client_cnt; ++i) {
        write_len = write(client_socks[i], msg, len);
        if (write_len < 0) {
            printf("send msg to client-%d failed: %s\n", client_socks[i], strerror(errno));
        }
    }

    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *arg) {
    int client_socket = *((int *) arg);
    char msg[BUFF_SIZE];
    ssize_t read_len;
    while (1) {
        bzero(msg, sizeof(msg));
        read_len = read(client_socket, msg, sizeof(msg));
        if (read_len < 0) {
            printf("client-%d read msg failed: %s\n", client_socket, strerror(errno));
            break;
        } else if (0 == read_len) {
            printf("client-%d has been closed\n", client_socket);
            break;
        }
        send_msg_to_all(msg, read_len);
    }

    // 移除关闭的 client socket
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_cnt; ++i) {
        if (client_socket == client_socks[i]) {
            for (int j = i + 1; j < client_cnt; ++j) {
                client_socks[j - 1] = client_socks[j];
            }
            break;
        }
    }
    client_cnt--;
    pthread_mutex_unlock(&mutex);

    printf("-----------debug----------begin\n");
    for (int i = 0; i < client_cnt; ++i) {
        printf("client[%d]==[%d]\n", i, client_socks[i]);
    }
    printf("-----------debug---------end\n");

    close(client_socket);

    return NULL;
}