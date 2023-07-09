//
// Created by demon on 7/9/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 64
#define PTHREAD_NUM 2
int sum = 0;

void *thread_inc(void *arg);

void *thread_des(void *arg);

int main(int argc, char *argv[]) {
    pthread_t t_ids[PTHREAD_NUM];
    // 把轮询次数加大才能模拟出并发问题
    int pthread_param = 1000000;
    void *pthread_result;

    for (int i = 0; i < PTHREAD_NUM; i++) {
        int thread_ret;
        if (0 == i % 2) {
            thread_ret = pthread_create(&t_ids[i], NULL, thread_inc, (void *) &pthread_param);
            if (0 != thread_ret) {
                puts("create inc pthread failed");
                return -1;
            }
        } else {
            thread_ret = pthread_create(&t_ids[i], NULL, thread_des, (void *) &pthread_param);
            if (0 != thread_ret) {
                puts("create des pthread failed");
                return -1;
            }
        }
        printf("create thread: %ld success...\n", t_ids[i]);
    }

    for (int i = 0; i < PTHREAD_NUM; i++) {
        int join_ret = pthread_join(t_ids[i], &pthread_result);
        if (0 != join_ret) {
            printf("waiting pthread:[%ld] failed\n", t_ids[i]);
            continue;
        }
        printf("get result from pthread[%ld] is: [%s]\n", t_ids[i], (char *) pthread_result);
        free(pthread_result);
    }

    printf("sum==[%d], end main thread: %ld for process: %d...\n", sum, pthread_self(), getpid());
    return 0;
}

void *thread_inc(void *arg) {
    int cnt = *((int *) (arg));
    for (int j = 0; j < cnt; ++j) {
        sum += 1;
    }
    printf("end thread_inc thread: %ld for process: %d...\n", pthread_self(), getpid());

    char *result_msg = (char *) malloc((sizeof(char) * BUFFER_SIZE));
    bzero(result_msg, BUFFER_SIZE);
    sprintf(result_msg, "thread_inc.sum==%d", sum);

    return (void *) result_msg;
}

void *thread_des(void *arg) {
    int cnt = *((int *) (arg));
    for (int j = 0; j < cnt; ++j) {
        sum -= 1;
    }
    printf("end thread_des thread: %ld for process: %d...\n", pthread_self(), getpid());

    char *result_msg = (char *) malloc((sizeof(char) * BUFFER_SIZE));
    bzero(result_msg, BUFFER_SIZE);
    sprintf(result_msg, "thread_des.sum==%d", sum);

    return (void *) result_msg;
}
