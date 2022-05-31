FLAGS = -Wall

run:
	gcc $(FLAGS) cook.c -o cook
	gcc $(FLAGS) deliverer.c -o deliverer
	gcc $(FLAGS) main.c -o main
	make clean

tests:
	./main 5 5

clean:
	rm -f *.o