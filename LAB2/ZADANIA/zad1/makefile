FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make test_all

test_all:
	./main from.txt to.txt
	./main from.txt to.txt
	./main fsd.txt
	./main fsd.txt to.txt

clean:
	rm -f *.o