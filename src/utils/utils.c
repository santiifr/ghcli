#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_current_branch() {
    static char branch[MAX_OUTPUT];
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD", "r");
    if (!fp) return NULL;
    if (fgets(branch, MAX_OUTPUT, fp)) {
        branch[strcspn(branch, "\n")] = 0;
    }
    pclose(fp);
    return branch;
}

char *get_repo_name() {
    static char repo[MAX_OUTPUT];
    FILE *fp = popen("basename -s .git `git config --get remote.origin.url`", "r");
    if (!fp) return NULL;
    if (fgets(repo, MAX_OUTPUT, fp)) {
        repo[strcspn(repo, "\n")] = 0;
    }
    pclose(fp);
    return repo;
}

char *get_repo_owner() {
    static char owner[MAX_OUTPUT];
    FILE *fp = popen("git remote get-url origin", "r");
    if (!fp) return NULL;

    char url[MAX_OUTPUT];
    if (!fgets(url, MAX_OUTPUT, fp)) {
        pclose(fp);
        return NULL;
    }
    pclose(fp);

    url[strcspn(url, "\n")] = 0;

    char *start = strstr(url, "github.com/");
    if (start) {
        start += strlen("github.com/");
    } else if ((start = strchr(url, ':'))) {
        start += 1;
    } else {
        return NULL;
    }

    char *slash = strchr(start, '/');
    if (!slash) return NULL;
    *slash = '\0';

    strncpy(owner, start, MAX_OUTPUT);
    return owner;
}


char *get_github_token() {
    return getenv("GITHUB_TOKEN");
}
