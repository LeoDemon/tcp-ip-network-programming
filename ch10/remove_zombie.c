//
// Created by DemonLee on 10/19/22.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

int child(int k) {
    printf("I am child-%d: %d\n", k, getpid());
    sleep(10 + k);
    printf("child-%d: %d exit now\n", k, getpid());
    exit(k);
}

void parent(int flag) {
    printf("I am parent-%d\n", getpid());
    if (flag) {
        return;
    }
    while (1) {
        printf("parent is going to sleep...\n");
        sleep(5);
    }
}

void recycle_child(int signal) {
    printf("receive signal: %d\n", signal);
    int status;
    pid_t child_pid;
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("recycle child: %d, state: %d\n", child_pid, WEXITSTATUS(status));
    }
}

int main() {
    struct sigaction act;
    act.__sigaction_u.__sa_handler = recycle_child;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    // comment this line for testing zombie process recycle
    sigaction(SIGCHLD, &act, 0);

    pid_t pid;
    for (int i = 0; i < 5; i++) {
        pid = fork();
        if (0 == pid) {
            child(i);
        }
    }

    // parent(1);
    parent(0);

    return 0;
}
