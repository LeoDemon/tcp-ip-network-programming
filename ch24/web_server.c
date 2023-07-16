//
// Created by demon on 7/13/23.
//

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100
#define MAX_CLIENT 128

void *request_handler(void *arg);

char *content_type(char *file);

void send_error(FILE *fp);

void send_data(FILE *fp, char *content_type, char *file_name);

void error_handling(char *message, int sock);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        return -1;
    }
    struct sockaddr_in server_sockaddr, client_sockaddr;
    memset(&server_sockaddr, '\0', sizeof(server_sockaddr));
    socklen_t client_sockaddr_len = sizeof(client_sockaddr);

    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        printf("create socket failed: [%s]\n", strerror(errno));
        return -2;
    }

    int reuse = 1;
    int ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0) {
        error_handling("set socket option failed", server_sock);
    }

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr.sin_port = htons(atoi(argv[1]));

    ret = bind(server_sock, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr));
    if (ret < 0) {
        error_handling("bind socket failed", server_sock);
    }

    ret = listen(server_sock, MAX_CLIENT);
    if (ret < 0) {
        error_handling("listen socket failed", server_sock);
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

        ret = pthread_create(&t_id, NULL, request_handler, (void *) &client_socket);
        if (ret < 0) {
            printf("create thread for client-%d failed: %s", client_socket, strerror(errno));
            continue;
        }

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

void *request_handler(void *arg) {
    int client_sock = *((int *) arg);
    FILE *client_read = fdopen(client_sock, "r");
    FILE *client_write = fdopen(dup(client_sock), "w");

    char req_line[SMALL_BUF];
    bzero(req_line, sizeof(req_line));
    fgets(req_line, SMALL_BUF, client_read);
    if (strstr(req_line, "HTTP/") == NULL) {
        send_error(client_write);
        fclose(client_read);
        fclose(client_write);
        return NULL;
    }

    char method[10], file_name[30], content_type_name[15];
    bzero(method, sizeof(method));
    bzero(file_name, sizeof(file_name));
    bzero(content_type_name, sizeof(content_type_name));
    strcpy(method, strtok(req_line, " /"));
    strcpy(file_name, strtok(NULL, " /"));
    strcpy(content_type_name, content_type(file_name));

    if (0 != strcmp("GET", method)) {
        send_error(client_write);
        fclose(client_read);
        fclose(client_write);
        return NULL;
    }
    fclose(client_read);
    send_data(client_write, content_type_name, file_name);
    close(client_sock);
    return NULL;
}

void send_data(FILE *fp, char *content_type, char *file_name) {
    char protocol[] = "HTTP/1.1 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char content_len[SMALL_BUF];
    char content_type_header[SMALL_BUF];
    char buf[BUF_SIZE];

    bzero(content_type_header, sizeof(content_type_header));
    bzero(buf, sizeof(buf));
    bzero(content_len, sizeof(content_len));
    sprintf(content_type_header, "Content-type:%s\r\n\r\n", content_type);

    FILE *send_file = fopen(file_name, "r");
    if (NULL == send_file) {
        send_error(fp);
        return;
    }

    struct stat st;
    stat(file_name, &st);
    sprintf(content_len, "Content-length:%ld\r\n", st.st_size);

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(content_len, fp);
    fputs(content_type_header, fp);
    while (NULL != fgets(buf, BUF_SIZE, send_file)) {
        fputs(buf, fp);
        fflush(fp);
    }

    fflush(fp);
    fclose(fp);
}

char *content_type(char *file) {
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    bzero(extension, sizeof(extension));
    bzero(file_name, sizeof(file_name));

    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));

    printf("file_name: [%s], extension: [%s]\n", file_name, extension);

    if (strcmp(extension, "html") == 0 || strcmp(extension, "htm") == 0) {
        return "text/html";
    }

    return "text/plain";
}

void send_error(FILE *fp) {
    char protocol[] = "HTTP/1.1 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char content_len[] = "Content-length:2048\r\n";
    char content_type[] = "Content-type:text/html\r\n\r\n";

    char content[] = "<html>"
                     "<head><title>Network</title></head>"
                     "<body><font size=+2><br>Error, pls check request method and file name!</font></body>"
                     "</html>";

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(content_len, fp);
    fputs(content_type, fp);
    fputs(content, fp);

    fflush(fp);
}

void error_handling(char *msg, int sock) {
    int errnum = errno;
    printf("%s: [%s]\n", msg, strerror(errnum));
    close(sock);
    exit(errnum);
}
