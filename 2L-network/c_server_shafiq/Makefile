CC = gcc
CFLAGS = -Wall -I/usr/include/mysql
LIBS = -lmysqlclient -lpthread

all: server

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(LIBS)

clean:
	rm -f server