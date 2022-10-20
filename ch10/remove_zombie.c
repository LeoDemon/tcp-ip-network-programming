//
// Created by DemonLee on 10/19/22.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

void recycle_child(int signal) {
    printf("receive signal: %d\n", signal);
    int status;
    pid_t child_pid;
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("pid-%d recycle child: %d, state: %d\n", getpid(), child_pid, WEXITSTATUS(status));
    }
}

void register_signal() {
    struct sigaction act;
    act.__sigaction_u.__sa_handler = recycle_child;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);
}

void register_signal_default() {
    struct sigaction act;
    act.__sigaction_u.__sa_handler = NULL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);
}

int child(int k) {
    printf("I am child-%d: %d\n", k, getpid());

    if (k == 3) {
        pid_t child_pid = fork();
        if (0 == child_pid) {
            // 由于子进程继承了父进程的全部能力，故默认情况下，子进程也有 recycle_child 的能力，因而该孙子进程能被子进程回收
            child(100);
        } else {
            register_signal_default(); // 屏蔽该子进程的回收能力
            printf("child-%d create a new child: %d\n", getpid(), child_pid);
        }
    }

    if (k == 100) {
        sleep(100);
    } else {
        sleep(60 + k);
    }
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

int main() {
    register_signal();

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
