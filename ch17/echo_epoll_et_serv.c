//
// Created by DemonLee on 11/15/22.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 8
#define EPOLL_SIZE 50

// 将文件描述符调整为非阻塞模式：边缘触发模式下的必须操作
void set_non_blocking_mode(int fd) {
    // 获取当前文件描述符的属性
    int flag = fcntl(fd, F_GETFL, 0);
    // 在当前属性的基础上增加非阻塞模式
    int new_flag = flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);

    printf("change fd:[%d] from %d to %d\n", fd, flag, new_flag);
}

void run(int epfd, struct epoll_event *event) {
    char buf[BUFFER_SIZE];
    ssize_t len;
    ssize_t w_len;

    int client_fd = event->data.fd;
    // 边缘触发需要增加 while(1) 循环，因为输入缓冲区有数据时，内核只会注册一次事件
    while (1) {
        bzero(buf, sizeof(buf));
        len = read(client_fd, buf, BUFFER_SIZE);
        if (len > 0) {
            buf[len] = '\0';
            if (buf[len - 1] == '\n') {
                buf[len - 1] = '\0';
            }
            printf("message from client: [%s], len==[%ld]\n", buf, len);

            size_t len2 = strlen(buf);
            w_len = write(client_fd, buf, len2);
            if (w_len != len2) {
                printf("write back error: %s\n", strerror(errno));
                break;
            }
        } else if (len == 0) {
            printf("client socket has been closed.\n");
            break;
        } else {
            // len == -1 and errno == EAGAIN: 输入缓冲区的数据已全部读取完成
            if (errno == EAGAIN) {
                printf("read from socket input buffer end...\n");
                return;
            }
            printf("read from socket error: %s, ignore this on non-blocking mode...\n", strerror(errno));
        }
    }

    epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    printf("close client socket: [%d]\n", client_fd);
}

int accept_request(int epfd, struct epoll_event *event, int listen_fd, struct sockaddr_in *client_addr,
                   socklen_t *client_addr_len) {
    int client_fd = accept(listen_fd, (struct sockaddr *) client_addr, client_addr_len);
    if (client_fd < 0) {
        printf("accept client_fd error: %s\n", strerror(client_fd));
        return -1;
    }
    // 给当前新连接增加非阻塞模式
    set_non_blocking_mode(client_fd);

    bzero(event, sizeof(struct epoll_event));
    // 设置为 EPOLLET 边缘触发
    event->events = EPOLLIN | EPOLLET;
    event->data.fd = client_fd;
    int epoll_ret = epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, event);
    if (epoll_ret < 0) {
        printf("epoll add client_fd error: %s\n", strerror(client_fd));
        return -1;
    }

    printf("connected client: %d\n", client_fd);
    return 0;
}

int prepare_socket(struct sockaddr_in *serv_addr, socklen_t serv_addr_len) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        exit(-1);
    }

    int reuse = 1;
    int ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0) {
        printf("set socket reuse addr error: %s\n", strerror(errno));
        exit(-1);
    }

    ret = bind(listen_fd, (struct sockaddr *) serv_addr, serv_addr_len);
    if (ret < 0) {
        printf("socket bind error: %s\n", strerror(errno));
        exit(-1);
    }

    ret = listen(listen_fd, 1024);
    if (ret < 0) {
        printf("socket bind error: %s\n", strerror(errno));
        exit(-1);
    }

    return listen_fd;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s <Port>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in serv_sock_addr;
    bzero(&serv_sock_addr, sizeof(serv_sock_addr));
    serv_sock_addr.sin_family = AF_INET;
    serv_sock_addr.sin_port = htons(atoi(argv[1]));
    serv_sock_addr.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);

    int listen_fd = prepare_socket(&serv_sock_addr, sizeof(serv_sock_addr));

    // fd_set reads, temps;
    // struct timeval timeout;
    // FD_ZERO(&reads);
    // FD_SET(listen_fd, &reads);
    // int fd_max = listen_fd;

    int epfd = epoll_create(EPOLL_SIZE);
    struct epoll_event *ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    struct epoll_event event;

    // 给监听 socket 增加非阻塞模式
    set_non_blocking_mode(listen_fd);

    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &event);

    int event_cnt, i;
    while (1) {
        // fd_num = select(fd_max + 1, &temps, NULL, NULL, &timeout);
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (-1 == event_cnt) {
            printf("epoll_wait() error: %s\n", strerror(errno));
            break;
        }
        printf("get epoll events from epoll_wait func: %d\n", event_cnt);
        for (i = 0; i < event_cnt; i++) {
            if (listen_fd == ep_events[i].data.fd) {
                accept_request(epfd, &event, listen_fd, &client_addr, &client_addr_len);
                continue;
            }
            run(epfd, &ep_events[i]);
        }
    }

    close(listen_fd);
    close(epfd);

    return 0;
}
