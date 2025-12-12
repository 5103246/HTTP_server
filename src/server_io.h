#ifndef SERVER_IO_H
#define SERVER_IO_H

char *ReceiveRequest(int client_socket);
char *BuildResponse(const char *body);
int SendResponse(int client_socket, char *response);

#endif /* SERVER_IO_H */