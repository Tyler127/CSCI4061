lab6_p1: p1.c
	gcc -o main p1.c
	./main

lab6_p2: p2.c
	gcc -o main p2.c
	./main

lab6_p3_1: p3.c
	gcc -o main p3.c
	./main

lab6_p3_2: p3.c
	gcc -g -o main p3.c
	valgrind --leak-check=full ./main

lab6_p3_3: p3.c
	gcc -g -o main -fsanitize=address p3.c
	./main


.PHONY: clean

clean:
	rm -f main