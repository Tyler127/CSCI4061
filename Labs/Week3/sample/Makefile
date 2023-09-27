############ Processes ############
# wait
lab3_p1: p1.c
	gcc -o main p1.c
	./main

# exec
lab3_p2: p2.c
	gcc -o main p2.c
	./main

lab3_p3: p3.c
	gcc -o main p3.c
	./main

############ File I/O ############
lab3_p4: p4.c
	rm -f out.txt
	gcc -o main p4.c

lab3_p4_1: lab3_p4
	./main out.txt 1

lab3_p4_2: lab3_p4
	./main out.txt 2

lab3_p4_3: lab3_p4
	./main out.txt 3


.PHONY: clean
clean:
	rm -f main out.txt