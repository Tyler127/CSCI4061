# Run ./server and ./client in separate terminals, no targets to run the code provided
all: server client

server: server.c
	gcc -o server server.c

client: client.c
	gcc -o client client.c

.PHONY: clean

clean:
	rm -f server client