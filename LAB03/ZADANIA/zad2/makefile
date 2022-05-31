FLAGS = -Wall

run:
	make delete_txt
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make test_all

test_all:
	./main d
	./main asc sds
	./main 2 sds
	./main 69429 30
	./main 7346 21
	./main 9999 4
	./main 10000000 32

clean:
	rm -f *.o

delete_txt:
	rm -f w*.txt