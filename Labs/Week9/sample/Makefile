CC=gcc
CFLAGS= -pthread

lab9_p1: p1.c
	$(CC) $(CFLAGS) -o main $^
	./main 100 10

lab9_p2: p2.c
	$(CC) $(CFLAGS) -o main $^
	./main 5

.PHONY: clean

clean:
	rm -f main