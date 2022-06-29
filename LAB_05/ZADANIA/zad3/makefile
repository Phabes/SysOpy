FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	gcc $(FLAGS) -c consumer.c
	gcc $(FLAGS) consumer.o -o consumer
	gcc $(FLAGS) -c producer.c
	gcc $(FLAGS) producer.o -o producer
	make clean
	make tests

tests:
	./main

clean:
	rm -f *.o