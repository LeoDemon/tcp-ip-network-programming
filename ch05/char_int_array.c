//
// Created by DemonLee on 6/6/22.
//

#include <stdio.h>
#include <string.h>

// for testing int convert to char array.
int main(int argc, char **argv) {
    printf("aaa");

    char aa[64];
    bzero(aa, sizeof(aa));

    int a = 100, b = 200, c = 330;

    ((int *) &aa)[0] = a;
    ((int *) &aa)[1] = b;
    ((int *) &aa)[2] = c;

    printf("aa=[%s]\n", aa);
    printf("aa0=[%d]\n", ((int *) &aa)[0]);
    printf("aa1=[%d]\n", ((int *) &aa)[1]);
    printf("aa2=[%d]\n", ((int *) &aa)[2]);

    return 0;
}