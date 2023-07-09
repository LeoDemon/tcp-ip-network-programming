//
// Created by demon on 7/9/23.
//

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define LOOP_SIZE 5
#define THREAD_NUM 2

void *read(void *arg);
void *accu(void *arg);

static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[]) {
    // 初始化信号量
    int sem_ret = sem_init(&sem_one, 0, 0);
    if (0 != sem_ret) {
        puts("init semaphore-1 failed");
        return -1;
    }
    sem_ret = sem_init(&sem_two, 0, 1);
    if (0 != sem_ret) {
        puts("init semaphore-2 failed");
        return -1;
    }

    int pthread_ret;
    pthread_t t_ids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (0 == i % 2) {
            pthread_ret = pthread_create(&t_ids[i], NULL, read, NULL);
        } else {
            pthread_ret = pthread_create(&t_ids[i], NULL, accu, NULL);
        }
        if (0 != pthread_ret) {
            puts("create pthread failed");
            return -2;
        }
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_ret = pthread_join(t_ids[i], NULL);
        if (0 != pthread_ret) {
            puts("join pthread failed");
            return -3;
        }
    }

    // 销毁信号量
    sem_destroy(&sem_one);
    sem_destroy(&sem_two);

    return 0;
}

void *read(void *arg) {
    for (int i = 0; i < LOOP_SIZE; i++) {
        fputs("Input num: ", stdout);
        // 先读取数据
        sem_wait(&sem_two);

        scanf("%d", &num);

        // 再进行计算
        sem_post(&sem_one);
    }
}

void *accu(void *arg) {
    int sum = 0;
    for (int i = 0; i < LOOP_SIZE; ++i) {
        // 等待从外部输入数据到内存
        sem_wait(&sem_one);

        sum += num;

        // 计算结束，可以进行下一轮输入
        sem_post(&sem_two);
    }
    printf("accu sum=[%d]\n", sum);
}
