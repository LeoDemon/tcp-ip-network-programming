//
// Created by DemonLee on 10/20/22.
//

#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

void read_routine(int client_fd) {
    char buf[BUFFER_SIZE];
    bzero(buf, sizeof(buf));
    ssize_t len;

    while (1) {
        bzero(buf, sizeof(buf));
        len = read(client_fd, buf, BUFFER_SIZE);
        if (len == 0) {
            // End-of-File，socket has been closed
            printf("server socket has been closed.\n");
            break;
        } else if (len < 0) {
            if (EINTR == errno) {
                // non-blocking, continue reading
                continue;
            }
            printf("read from socket error: %s\n", strerror(errno));
            break;
        }
        buf[len] = '\0';
        printf("message from server: [%s]\n", buf);
    }
}

void write_routine(int sock_fd) {
    char buf[BUFFER_SIZE];
    ssize_t w_len;
    size_t len;
    char *input_ptr = NULL;
    int flag = 0;

    while (1) {
        bzero(buf, sizeof(buf));
        input_ptr = fgets(buf, sizeof(buf), stdin);
        if (NULL == input_ptr) {
            printf("get no data from stdin\n");
            flag = 1;
            break;
        } else if (strcmp("q\n", buf) == 0 || strcmp("Q\n", buf) == 0) {
            printf("client quit.\n");
            break;
        }
        printf("read data from stdin: %s", buf);
        len = strlen(buf);
        w_len = write(sock_fd, buf, len);
        if (w_len != len) {
            printf("write data to server error: %s\n", strerror(errno));
        }
    }
    // only shutdown socket write stream, ensure read stream can be continued
    shutdown(sock_fd, SHUT_WR);
    exit(flag);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: echo_mpserv <IP> <Port>\n");
        return 1;
    }
    register_signal();

    struct sockaddr_in serv_sock;
    bzero(&serv_sock, sizeof(serv_sock));
    serv_sock.sin_family = AF_INET;
    serv_sock.sin_port = htons(atoi(argv[2]));
    serv_sock.sin_addr.s_addr = inet_addr(argv[1]);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("create socket error: %s\n", strerror(errno));
        exit(-1);
    }
    int ret = connect(sock_fd, (struct sockaddr *) &serv_sock, sizeof(serv_sock));
    if (ret < 0) {
        printf("connect to server error: %s\n", strerror(errno));
        exit(-1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        write_routine(sock_fd);
    } else {
        read_routine(sock_fd);
        // ensure child process can be killed
        // printf("parent is sleeping now...\n");
        // sleep(1);
        ret = kill(pid, SIGTERM);
        if (ret < 0) {
            printf("kill error: %s\n", strerror(errno));
        }
    }
    close(sock_fd);

    return 0;
}
