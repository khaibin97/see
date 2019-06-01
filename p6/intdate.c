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
#include <time.h>

int su1counter;
void handler_SIGUSR1() {
    printf("SIGUSR1 caught!\n");
    su1counter++;
}

void handler_SIGALRM() {
    time_t currtime;
    time(&currtime);
    printf("PID: %d | Current Time: %s"
        , getpid(), ctime(&currtime));
    alarm(3);
}

void handler_SIGINT() {
    printf("\nSIGINT received.\nSIGUSR1 was received %d times."
        , su1counter);
    printf(" Exiting now.\n");
    exit(0);
}

int main() {
    struct sigaction act;
    memset (&act, 0, sizeof(act));
    act.sa_handler = handler_SIGALRM;
    act.sa_flags = 0;
    if (sigaction(SIGALRM, &act, NULL) != 0) {
        printf("Error: Building SIGALRM handler\n");
        exit(1);
    }

    struct sigaction act2;
    memset (&act2, 0, sizeof(act2));
    act2.sa_handler = handler_SIGUSR1;
    act2.sa_flags = 0;
    if (sigaction(SIGUSR1, &act2, NULL) != 0) {
        printf("Error: Building SIGUSR1 handler\n");
        exit(1);
    }

    struct sigaction act3;
    memset (&act3, 0, sizeof(act3));
    act3.sa_handler = handler_SIGINT;
    act3.sa_flags = 0;
    if (sigaction(SIGINT, &act3, NULL)) {
        printf("Error: Building SIGINT handler\n");
        exit(1);
    }

    printf("Pid and time will be printed every 3 seconds.\n");
    printf("Enter ^C to end the program.\n");
    alarm(3);
    while (1) {
    }
    return 0;
}


