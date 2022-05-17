FLAGS = -Wall

run:
	gcc $(FLAGS) cook.c -o cook -lrt -lpthread
	gcc $(FLAGS) deliverer.c -o deliverer -lrt -lpthread
	gcc $(FLAGS) main.c -o main -lrt -lpthread
	make clean

tests:
	./main 5 5

clean:
	rm -f *.o