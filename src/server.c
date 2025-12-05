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
#define BUF_SIZE 1024

void HandleClient(int client_socket) {
    char *buf = (char *)malloc(BUF_SIZE);
    if (buf == NULL) {
        perror("malloc failed in HandleClient");
        close(client_socket);
        return;
    }
    ssize_t recv_size = 0;
    if ((recv_size = recv(client_socket, buf, BUF_SIZE - 1, 0)) == -1) {
        const int error = errno;
        if (error != EINTR) {
            perror("recv failed");
        }
    }

    buf[recv_size] = '\0';
    printf("receive : %s\n", buf);
    if (send(client_socket, buf, recv_size, 0) == -1) {
        const int error = errno;
        if (error != EINTR) {
            perror("send failed");
        }
    }
    free(buf);
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
        HandleClient(client_socket);
    }

    close(server_socket);
    return 0;
}