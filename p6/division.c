/* Name:       Khai Bin Woon
 * CS login:   khai@cs.wisc.edu
 * Section(s): 002
 * 
 * Copyright 2018 khaibin_woon
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int divCounter;
void handler_SIGINT() {
    printf("\nTotal number of operations completed");
    printf(" succesfully: %d\n", divCounter);
    printf("The program will be terminated.\n");
    exit(0);
}

void handler_SIGFPE() {
    printf("Error: a division by 0 operation was attempted.");
    handler_SIGINT();
}

int main(int argc, char *argv[]) {
    char buf[100];
    int first, second;

    struct sigaction act;
    memset (&act, 0, sizeof(act));
    act.sa_handler = handler_SIGFPE;
    act.sa_flags = 0;
    if ( sigaction(SIGFPE, &act, NULL) != 0 ) {
        printf("Error: Building SIGFPE handler\n");
        exit(1);
    }

    struct sigaction act2;
    memset (&act2, 0, sizeof(act2));
    act2.sa_handler = handler_SIGINT;
    act2.sa_flags = 0;
    if ( sigaction(SIGINT, &act2, NULL) ) {
        printf("Error: Building SIGINT handler\n");
        exit(1);
    }

    while (1) {
        printf("Enter first integer: ");
        first = atoi(fgets(buf, 100, stdin));
        printf("Enter second integer: ");
        second = atoi(fgets(buf, 100, stdin));

        printf("%d / %d is %d with a remainder of %d\n"
            , first, second, first/second, first%second);

        divCounter++;
    }
}
