//
// Created by DemonLee on 10/20/22.
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

void run(int listen_fd, int client_fd, int pipe_fd) {
    printf("child: %d response the client request...\n", getpid());
    close(listen_fd);

    char buf[BUFFER_SIZE];
    ssize_t len;
    ssize_t w_len;
    int flag = 0;

    while (1) {
        bzero(buf, sizeof(buf));
        len = read(client_fd, buf, BUFFER_SIZE);
        if (len == 0) {
            // End-of-File，socket has been closed
            printf("client socket has been closed.\n");
            break;
        } else if (len < 0) {
            if (EINTR == errno) {
                // non-blocking, continue reading
                continue;
            }
            printf("read from socket error: %s\n", strerror(errno));
            flag = -1;
            break;
        }
        buf[len] = '\0';

        write(pipe_fd, buf, len);

        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
        printf("message from client: [%s]\n", buf);

        w_len = write(client_fd, buf, len);
        if (w_len != len) {
            printf("write back error: %s\n", strerror(errno));
            flag = -2;
            break;
        }
    }
    close(client_fd);
    exit(flag);
}

int prepare_socket(struct sockaddr_in *serv_addr, socklen_t serv_addr_len) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        exit(-1);
    }

    int ret = bind(listen_fd, (struct sockaddr *) serv_addr, serv_addr_len);
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

void store(int pipe_fd) {
    char buf[BUFFER_SIZE];
    bzero(buf, sizeof(buf));

    ssize_t len;
    FILE *fp = fopen("./echo_msg.txt", "wt");
    for (int i = 0; i < 10; ++i) {
        len = read(pipe_fd, buf, BUFFER_SIZE);
        fwrite(buf, 1, len, fp);
        fflush(fp);
    }
    fclose(fp);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: echo_mpserv <Port>\n");
        return 1;
    }
    register_signal();

    struct sockaddr_in serv_sock;
    bzero(&serv_sock, sizeof(serv_sock));
    serv_sock.sin_family = AF_INET;
    serv_sock.sin_port = htons(atoi(argv[1]));
    serv_sock.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);

    int fds[2];
    pipe(fds);
    pid_t child_pid = fork();
    if (0 == child_pid) {
        store(fds[0]);
    }

    int listen_fd = prepare_socket(&serv_sock, sizeof(serv_sock));
    while (1) {
        int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_fd < 0) {
            printf("accept client_fd error: %s\n", strerror(client_fd));
            continue;
        }
        child_pid = fork();
        if (0 == child_pid) {
            run(listen_fd, client_fd, fds[1]);
        } else {
            // child_pid > 0 || child_pid == -1
            close(client_fd);
        }
    }

    close(listen_fd);
    return 0;
}
