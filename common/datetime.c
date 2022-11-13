//
// Created by DemonLee on 5/24/22.
//

#include "datetime.h"
#include "log.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

char *now(char *time_buffer) {
    if (NULL == time_buffer) {
        fputs("time_buffer is NULL", stderr);
        return NULL;
    }
    time_t timer;
    struct tm *tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    return time_buffer;
}

void display_now() {
    char nowTime[30];
    memset(nowTime, '\0', sizeof(nowTime));
    now(nowTime);
    printf("current time: [%s]\n", nowTime);
}
