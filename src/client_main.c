#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define PORT 8080
#define RECEIVE_CAPACITY 1024
#define REMAINING 512

int SendRequest(int client_socket, char *request) {
    size_t request_len = strlen(request);
    size_t total_size = 0;
    while (total_size < request_len) {
        ssize_t send_size = send(client_socket, request, request_len, 0);
        if (send_size < 0) {
            const int error = errno;
            if (error == EINTR) {
                continue;
            }
            perror("send failed");
            close(client_socket);
            return EXIT_FAILURE;
        }
        total_size += send_size;
    }
    return 0;
}

char *ReceiveResponse(int client_socket) {
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

        if (strstr(buf, "\r\n\r\n") != NULL) {
            break;
        }
    }
    buf[total_size] = '\0';
    return buf;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("two commands are required\n");
        return EXIT_FAILURE;
    }
    const int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(HOST);

    while (1) {
        if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            const int error = errno;
            if (error == EINTR) {
                continue;
            }
            perror("connect failed");
        }
        break;
    }

    char *request = argv[1];
    if (SendRequest(client_socket, request) != 0) {
        printf("send failed\n");
        close(client_socket);
        return EXIT_FAILURE;
    }

    char *server_response = ReceiveResponse(client_socket);
    if (!server_response) {
        printf("recv failed\n");
        close(client_socket);
        return EXIT_FAILURE;
    }
    printf("\n");
    printf("==================\n");
    printf("%s\n", server_response);
    printf("==================\n");

    free(server_response);
    close(client_socket);
    return 0;
}