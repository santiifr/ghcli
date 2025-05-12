#include "pr.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define MAX_SUMMARY 512

char *build_pr_url(const char *owner, const char *repo) {
    static char url[MAX_OUTPUT * 2];
    snprintf(
        url, sizeof(url),
        "https://api.github.com/repos/%s/%s/pulls?state=open",
        owner, repo
    );
    return url;
}

struct response_data {
    char *data;
    size_t size;
};

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;
    struct response_data *mem = (struct response_data *)userdata;
    mem->data = realloc(mem->data, mem->size + real_size + 1);
    memcpy(&(mem->data[mem->size]), ptr, real_size);
    mem->size += real_size;
    mem->data[mem->size] = 0;
    return real_size;
}

char *pr_summary() {
    static char summary[MAX_SUMMARY];
    summary[0] = '\0';

    char *branch = get_current_branch();
    char *repo = get_repo_name();
    char *owner = get_repo_owner();
    char *token = get_github_token();
    char *url = build_pr_url(owner, repo);

    if (!token || !branch || !repo || !owner || !url) {
        snprintf(summary, MAX_SUMMARY, "Missing required context or GitHub token.");
        return summary;
    }

    struct response_data chunk = {0};
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;

    if (!curl) {
        snprintf(summary, MAX_SUMMARY, "CURL init failed.");
        return summary;
    }

    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", token);

    headers = curl_slist_append(headers, "User-Agent: ghcli");
    headers = curl_slist_append(headers, auth_header);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        snprintf(summary, MAX_SUMMARY, "CURL error: %s", curl_easy_strerror(res));
        return summary;
    }

    cJSON *json = cJSON_Parse(chunk.data);
    free(chunk.data);

    if (!json || !cJSON_IsArray(json)) {
        snprintf(summary, MAX_SUMMARY, "Invalid JSON response.");
        cJSON_Delete(json);
        return summary;
    }

    int found = 0;
    for (int i = 0; i < cJSON_GetArraySize(json); i++) {
        cJSON *pr = cJSON_GetArrayItem(json, i);
        cJSON *head = cJSON_GetObjectItem(pr, "head");
        cJSON *head_ref = cJSON_GetObjectItem(head, "ref");
        if (head_ref && strcmp(head_ref->valuestring, branch) == 0) {
            int pr_number = cJSON_GetObjectItem(pr, "number")->valueint;
            const char *title = cJSON_GetObjectItem(pr, "title")->valuestring;

            char review_url[512];
            snprintf(review_url, sizeof(review_url),
                     "https://api.github.com/repos/%s/%s/pulls/%d/reviews",
                     owner, repo, pr_number);

            chunk.size = 0;
            chunk.data = NULL;
            curl = curl_easy_init();
            headers = curl_slist_append(NULL, "User-Agent: ghcli");
            headers = curl_slist_append(headers, auth_header);
            curl_easy_setopt(curl, CURLOPT_URL, review_url);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);

            int approvals = 0;
            if (res == CURLE_OK && chunk.data) {
                cJSON *reviews = cJSON_Parse(chunk.data);
                if (reviews && cJSON_IsArray(reviews)) {
                    for (int j = 0; j < cJSON_GetArraySize(reviews); j++) {
                        cJSON *review = cJSON_GetArrayItem(reviews, j);
                        cJSON *state = cJSON_GetObjectItem(review, "state");
                        if (state && strcmp(state->valuestring, "APPROVED") == 0) {
                            approvals++;
                        }
                    }
                    cJSON_Delete(reviews);
                }
            }
            free(chunk.data);

            snprintf(summary, MAX_SUMMARY,
                     "Branch: %s | PR: #%d | Title: %s | Approvals: %d ✅",
                     branch, pr_number, title, approvals);
            found = 1;
            break;
        }
    }

    if (!found) {
        snprintf(summary, MAX_SUMMARY, "No open PR found for branch '%s'", branch);
    }

    cJSON_Delete(json);
    return summary;
}
