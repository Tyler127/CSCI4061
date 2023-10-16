all: bugs.c
	gcc -g -o main bugs.c
	valgrind --leak-check=full ./main 3 3

.PHONY: clean

clean:
	rm -f main