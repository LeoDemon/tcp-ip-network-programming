//
// Created by DemonLee on 11/13/22.
//

#include <stdio.h>
#include "common/datetime.h"

#define BUF_SIZE 3

// standard io function
int main(int argc, char *argv[]) {
    FILE *fp1 = fopen("news-1.txt", "r");
    FILE *fp2 = fopen("stdcpy.txt", "w");

    display_now();

    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), fp1) != NULL) {
        fputs(buf, fp2);
    }

    display_now();

    fclose(fp1);
    fclose(fp2);

    return 0;
}
