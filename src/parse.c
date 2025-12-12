#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

void CleanupRequestLine (RequestLine *request_line) {
    if (!request_line) {
        return;
    }
    free(request_line->path);
    free(request_line->query);
    request_line->path = NULL;
    request_line->query = NULL;
}

int ParseURI(const char *uri, size_t uri_len, RequestLine *result) {
    const char *question = memchr(uri, '?', uri_len);
    
    size_t path_len = question - uri;
    if (!question) {
        path_len = uri_len;
    }
    if (path_len == 0) {
        return -1;
    }
    result->path = malloc(path_len + 1);
    if (!result->path) {
        return -1;
    }
    memcpy(result->path, uri, path_len);
    result->path[path_len] = '\0';

    size_t query_len = uri_len - path_len - 1;
    if (!question) {
        query_len = 0;
    }
    result->query = malloc(query_len + 1);
    if (!result->query) {
        free(result->path);
        result->path = NULL;
        return -1;
    }
    memcpy(result->query, question + 1, query_len);
    result->query[query_len] = '\0';

    return 0;
}

int ParseRequestLine(const char *request_line, RequestLine *result) {
    if (!request_line) {
        return -1;
    }

    const char *first_space = strchr(request_line, ' ');
    if (!first_space) {
        return -1;
    }
    const char *second_space = strchr(first_space + 1, ' ');
    if (!second_space) {
        return -1;
    }

    size_t method_len = first_space - request_line;
    if (method_len == 0 || method_len >= 16) {
        return -1;
    }
    memcpy(result->method, request_line, method_len);
    result->method[method_len] = '\0';

    const char *uri = first_space + 1;
    size_t uri_len = second_space - uri;
    if (uri_len == 0) {
        return -1;
    }
    if (ParseURI(uri, uri_len, result) == -1) {
        printf("parse URI failed");
        return -1;
    }

    const char *version = second_space + 1;
    size_t version_len = strlen(version);
    if (version_len == 0 || version_len >= 16) {
        CleanupRequestLine(result);
        return -1;
    }
    memcpy(result->version, version, version_len);
    result->version[version_len] = '\0';
    return 0;
}