/* Name:       Khai Bin Woon
 * CS login:   khai@cs.wisc.edu
 * Section(s): 002
 * 
 * Copyright 2018 khaibin_woon
 */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char **argv) {
    char c;
    pid_t pid;
    if (argc < 2) {
        printf("Usage: <signal> <pid>\n");
    }
    while ((c = getopt(argc, argv, "i:u:h")) != -1) {
        switch (c) {
            case 'i':
                pid = atoi(optarg);
                if (kill(pid, SIGINT) < 0) {
                    printf("Error: kill(SIGINT)\n");
                }
                break;
            case 'u':
                pid = atoi(optarg);
                if (kill(pid, SIGUSR1) < 0) {
                    printf("Error: kill(SIGUSR1)\n");
                }
                break;
            case 'h':
                printf("Usage: <signal> <pid>\n");
                exit(0);
            case '?':
                printf("Usage: <signal> <pid>\n");
                exit(1);
            default:
                printf("Usage: <signal> <pid>\n");
                exit(1);
        }
    }

    return 0;
}
