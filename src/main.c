#include <stdio.h>
#include <string.h>
#include "pr.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "pr") == 0) {
        char* summary = pr_summary();
        if (summary) {
            printf("PR Summary: %s\n", summary);
        } else {
            printf("Failed to get PR summary.\n");
        }
        return 0;
    }

    printf("Unknown command: %s\n", argv[1]);
    return 1;
}
