#ifndef PARSE_H
#define PARSE_H

#include <string.h>

typedef struct {
    char method[16];
    char *path;
    char *query;
    char version[16];
} RequestLine;

void CleanupRequestLine (RequestLine *request_line);
int ParseURI(const char *uri, size_t uri_len, RequestLine *result);
int ParseRequestLine(const char *request_line, RequestLine *result);

#endif /* PARSE_H */