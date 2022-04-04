FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make tests

tests:
	./main

clean:
	rm -f *.o