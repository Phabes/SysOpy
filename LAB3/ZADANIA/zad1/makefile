FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make test_all

test_all:
	./main 30
	./main sdvs
	./main sdvs 2
	./main 10

clean:
	rm -f *.o