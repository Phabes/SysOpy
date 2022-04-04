FLAGS = -Wall
FORK = fork
EXEC = exec
TASK = task

run:
	gcc $(FLAGS) -c $(FORK).c
	gcc $(FLAGS) $(FORK).o -o $(FORK)
	gcc $(FLAGS) -c $(TASK).c
	gcc $(FLAGS) $(TASK).o -o $(TASK)
	gcc $(FLAGS) -c $(EXEC).c
	gcc $(FLAGS) $(EXEC).o -o $(EXEC)
	make clean
	make tests FILE=$(FORK)
	make tests FILE=$(EXEC)

tests:
	./$(FILE) ignore
	./$(FILE) handler
	./$(FILE) mask
	./$(FILE) pending
	./$(FILE) fd
	./$(FILE)

clean:
	rm -f *.o