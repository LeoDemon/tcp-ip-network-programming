//
// Created by demon on 7/7/23.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_main(void *arg);

int main(int argc, char *argv[]) {
    pthread_t t_id;
    int pthread_param = 5;
    int thread_ret = pthread_create(&t_id, NULL, thread_main, (void *) &pthread_param);
    if (0 != thread_ret) {
        puts("create pthread failed");
        return -1;
    }
    printf("create thread: %ld success...\n", t_id);
    sleep(7);
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
    return NULL;
}