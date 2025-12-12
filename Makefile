SERVER=src/server 
CLIENT=src/client 

SERVER_SRCS=src/server_main.c src/server_io.c src/parse.c src/calc.c
CLIENT_SRCS=src/client_main.c

SERVER_OBJS=$(SERVER_SRCS:.c=.o)
CLIENT_OBJS=$(CLIENT_SRCS:.c=.o)

CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -g

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(SERVER_OBJS) $(CLIENT_OBJS)
