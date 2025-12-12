#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECEIVE_CAPACITY 1024
#define REMAINING 512
#define RESPONSE                                                          \
    "HTTP/1.1 200 OK\r\n"                                                 \
    "Content-Length: %zu\r\n"                                             \
    "\r\n%s"

char *ReceiveRequest(int client_socket) {
    size_t capacity = RECEIVE_CAPACITY;
    char *buf = malloc(capacity);
    if (!buf) {
        printf("malloc failed in ReceiveRequest\n");
        return NULL;
    }

    ssize_t total_size = 0;
    size_t remaining = REMAINING;
    while (1) {
        if (capacity - total_size < remaining) {
            capacity *= 2;
            char *temp = realloc(buf, capacity);
            if (!temp) {
                perror("realloc");
                free(buf);
                return NULL;
            }
            buf = temp;
        }
        ssize_t max_recv = capacity - total_size - 1;
        ssize_t recv_size = recv(client_socket, buf + total_size, max_recv, 0);

        if (recv_size == -1) {
            const int error = errno;
            if (error == EINTR) {
                continue;
            }
            perror("recv failed");
            free(buf);
            return NULL;
        }

        if (recv_size == 0) {
            if (total_size == 0) {
                free(buf);
                return NULL;
            }
            break;
        }

        total_size += recv_size;
        buf[total_size] = '\0';

        if (strstr(buf, "\r\n\r\n") != NULL) {
            printf("received full request\n");
            break;
        }
    }
    return buf;
}

char *BuildResponse(const char *body) {
    if (!body) {
        printf("no body\n");
        return NULL;
    }

    int responce_size = snprintf(NULL, 0, RESPONSE, strlen(body), body);
    if (responce_size < 0) {
        return NULL;
    }

    char *buf = malloc(responce_size + 1);
    if (!buf) {
        printf("malloc failed in BuildResponse\n");
        return NULL;
    }

    if (snprintf(buf, responce_size + 1, RESPONSE, strlen(body), body) < 0) {
        free(buf);
        return NULL;
    }
    return buf;
}

int SendResponse(int client_socket, char *response) {
    size_t response_len = strlen(response);

    size_t total_size = 0;
    while (total_size < response_len) {
        ssize_t send_size = send(client_socket, response, response_len, 0);
        if (send_size < 0) {
            const int error = errno;
            if (error == EINTR) {
                continue;
            }
            perror("send failed");
            return -1;
        }
        
        total_size += send_size;
    }
    return 0;
}