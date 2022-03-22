FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make test_all

test_all:
	./main a from.txt
	./main z from.txt
	./main vdsx no_file.txt
	./main vdsx from.txt
	./main x from.txt
	./main x no_file.txt
	./main A from.txt

clean:
	rm -f *.o