#------------File IO--------------
lab4_p1_1:
	gcc -DRDFILE -o main p1.c
	./main input1

lab4_p1_2:
	gcc -DRWFILE -o main p1.c
	./main input1 output1

#------------Redirection----------
lab4_p2_1: p2.c
	rm -f output2
	gcc -DCASE1 -o main p2.c
	./main < input2 > output2

lab4_p2_2: p2.c
	rm -f output2
	gcc -DCASE2 -o main p2.c
	./main < input2 > output2

lab4_p3: p3.c
	gcc -o main p3.c
	./main output3

#-----------Pipes-------------
lab4_p4: p4.c
	gcc -o main p4.c
	./main

.PHONY: clean

clean:
	rm -f main output1 output2 output3