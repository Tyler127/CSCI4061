# dependencies? answer: none?
linked_list.o:
	gcc -c linked_list.c

# dependencies? answer: main.c and linked_list.o <- made with prev. make command
# incomplete gcc command. answer: added linked_list.o to gcc recipe
main: main.c linked_list.o
	gcc -o main main.c linked_list.o

run: main
	./main

# used to specify targets without dependencies
.PHONY: clean

clean:
	rm linked_list.o main