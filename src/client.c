#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define HOST "127.0.0.1"
#define PORT 8080

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

    char *message = argv[1];
    printf("message : %s\n", message);
    ssize_t message_len = strlen(message);
    if (send(client_socket, message, message_len, 0) == -1) {
        const int error = errno;
        if (error != EINTR) {
            perror("send failed");
        }
    }

    char *buf = (char *)malloc(BUF_SIZE);
    if (buf == NULL) {
        perror("malloc failed in client");
        close(client_socket);
        return EXIT_FAILURE;
    }

    ssize_t recv_size = 0;
    if ((recv_size = recv(client_socket, buf, BUF_SIZE - 1, 0)) == -1) {
        const int error = errno;
        if (error != EINTR) {
            perror("recv failed");
        }
    }
    buf[recv_size] = '\0';
    printf("%s\n", buf);

    free(buf);
    close(client_socket);
    return 0;
}