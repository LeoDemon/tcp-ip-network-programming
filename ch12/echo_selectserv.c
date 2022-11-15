//
// Created by DemonLee on 10/23/22.
//

#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/errno.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#define BUFFER_SIZE 128

void read_child_proc(int sig) {
    pid_t pid;
    int state;
    while ((pid = waitpid(-1, &state, WNOHANG)) > 0) {
        printf("recycle child: %d, sig: %d, state: %d\n", pid, sig, WEXITSTATUS(state));
    }
}

void register_signal() {
    struct sigaction act;
    act.__sigaction_u.__sa_handler = read_child_proc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);
}

void run(int client_fd, fd_set *reads) {
    char buf[BUFFER_SIZE];
    ssize_t len;
    ssize_t w_len;

    bzero(buf, sizeof(buf));
    len = read(client_fd, buf, BUFFER_SIZE);
    if (len > 0) {
        buf[len] = '\0';
        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
        printf("message from client: [%s]\n", buf);

        w_len = write(client_fd, buf, len);
        if (w_len == len) {
            return;
        }
        printf("write back error: %s\n", strerror(errno));
    } else if (len == 0) {
        printf("client socket has been closed.\n");
    } else {
        printf("read from socket error: %s\n", strerror(errno));
    }
    FD_CLR(client_fd, reads);
    close(client_fd);
}

int
accept_request(fd_set *reads, int fd_max, int listen_fd, struct sockaddr_in *client_addr, socklen_t *client_addr_len) {
    int client_fd = accept(listen_fd, (struct sockaddr *) client_addr, client_addr_len);
    if (client_fd < 0) {
        printf("accept client_fd error: %s\n", strerror(client_fd));
        return fd_max;
    }
    FD_SET(client_fd, reads);
    if (fd_max < client_fd) {
        fd_max = client_fd;
    }
    printf("connected client: %d\n", client_fd);
    return fd_max;
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
    register_signal();

    struct sockaddr_in serv_sock_addr;
    bzero(&serv_sock_addr, sizeof(serv_sock_addr));
    serv_sock_addr.sin_family = AF_INET;
    serv_sock_addr.sin_port = htons(atoi(argv[1]));
    serv_sock_addr.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);

    int listen_fd = prepare_socket(&serv_sock_addr, sizeof(serv_sock_addr));

    fd_set reads, temps;
    struct timeval timeout;
    FD_ZERO(&reads);
    FD_SET(listen_fd, &reads);
    int fd_max = listen_fd;
    printf("fd_max: %d\n", fd_max);

    int fd_num, i;
    while (1) {
        temps = reads;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        fd_num = select(fd_max + 1, &temps, NULL, NULL, &timeout);
        if (-1 == fd_num) {
            printf("select() error: %s\n", strerror(errno));
            break;
        }
        if (0 == fd_num) {
            printf("select() timeout\n");
            continue;
        }
        for (i = 0; i < fd_max + 1; i++) {
            if (!FD_ISSET(i, &temps)) {
                printf("no event occurs on fd[%d]...\n", i);
                continue;
            }
            printf("event occurs on fd[%d]...\n", i);
            if (listen_fd == i) {
                // new request connection
                fd_max = accept_request(&reads, fd_max, listen_fd, &client_addr, &client_addr_len);
                continue;
            }
            // read message from client socket
            run(i, &reads);
        }
    }

    close(listen_fd);
    return 0;
}
