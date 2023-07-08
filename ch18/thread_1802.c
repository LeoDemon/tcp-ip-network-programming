//
// Created by demon on 7/7/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 64

void *thread_main(void *arg);

int main(int argc, char *argv[]) {
    pthread_t t_id;
    int pthread_param = 5;
    void *pthread_result;

    int thread_ret = pthread_create(&t_id, NULL, thread_main, (void *) &pthread_param);
    if (0 != thread_ret) {
        puts("create pthread failed");
        return -1;
    }
    printf("create thread: %ld success...\n", t_id);

    thread_ret = pthread_join(t_id, &pthread_result);
    if (0 != thread_ret) {
        puts("waiting pthread failed");
        return -2;
    }
    printf("get result from thread[%ld] is: [%s]\n", t_id, (char *) pthread_result);
    free(pthread_result);

    printf("end main thread: %ld for process: %d...\n", pthread_self(), getpid());
    return 0;
}

void *thread_main(void *arg) {
    int cnt = *((int *) (arg));
    for (int j = 0; j < cnt; ++j) {
        puts("running thread now...");
        sleep(1);
    }
    printf("end thread: %ld for process: %d...\n", pthread_self(), getpid());

    char *msg = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    bzero(msg, BUFFER_SIZE);
    strcpy(msg, "Hello, This is TCP/IP 网络编程实战...");

    return msg;
}