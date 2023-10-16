all: dir_trav.c
	gcc -o main dir_trav.c
	./main D1/f1.txt D1/D2/f1_hlink.txt D1/D4/f1_slink.txt D1

.PHONY: clean

clean:
	rm -f main D1/f1.txt D1/D2/f1_hlink.txt D1/D4/f1_slink.txt