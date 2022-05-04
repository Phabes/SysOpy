FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make tests

tests:
	./main
	./main wojtek TITLE MESSAGE
	./main nadawca
	./main data

clean:
	rm -f *.o