#include "calc.h"
#include "parse.h"
#include "server_io.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 5

void HandleClient(int client_socket) {
    char *request = NULL;
    RequestLine request_line;
    memset(&request_line, 0, sizeof(request_line));
    char *body = NULL;
    char *response = NULL;

    request = ReceiveRequest(client_socket);
    if (!request) {
        perror("ReceiveRequest failed");
        goto cleanup;
    }
    printf("====request====\n");
    printf("%s\n", request);
    printf("===============\n");
    printf("\n");

    if (ParseRequestLine(request, &request_line) == -1) {
        perror("ParseRequest falied");
        goto cleanup;
    }

    body = HandleCalculate(&request_line);
    if (!body) {
        perror("HandleCalculate falied");
        goto cleanup;
    }
   
    response = BuildResponse(body);
    if (!response) {
        perror("BuildResponse falied");
        goto cleanup;
    }
    printf("====response====\n");
    printf("%s\n", response);
    printf("================\n");
    printf("\n");

    if (SendResponse(client_socket, response) == -1) {
        perror("SendResponse falied");
        goto cleanup;
    }

cleanup:
    free(request);
    CleanupRequestLine(&request_line);
    free(body);
    free(response);
    close(client_socket);

}

int main() {
    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    if (listen(server_socket, BACKLOG) == -1) {
        perror("listen failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    while (1) {
        const int client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
        &client_len);
        if (client_socket == -1) {
            const int error = errno;
            if (error == EINTR) {
                continue;
            }
            perror("accept failed");
            break;
        }
        printf("Connection accepted from %s:%d\n", 
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("\n");
        HandleClient(client_socket);
    }

    close(server_socket);
    return 0;
}