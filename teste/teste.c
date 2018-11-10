#include "teste.h"
#include <stdio.h>

void start_test(char* title) {
    printf("\n%s\n==========================================================\n",
        title);
}

void assert(char* description, int assertion) {
    if (assertion) {
        printf("SUCCESS %s\n", description);
    } else {
        printf("FAIL %s\n", description);
    }
}

void end_test() {
    printf("==========================================================\n");
}