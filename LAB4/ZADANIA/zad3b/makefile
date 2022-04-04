FLAGS = -Wall
SENDER = sender
CATCHER = catcher

run:
	gcc $(FLAGS) -c $(SENDER).c
	gcc $(FLAGS) $(SENDER).o -o $(SENDER)
	gcc $(FLAGS) -c $(CATCHER).c
	gcc $(FLAGS) $(CATCHER).o -o $(CATCHER)
	make clean
	make tests

tests: # RUNS CATCHER IN SPECIFIC MODE
	./$(CATCHER) KILL
#	./$(CATCHER) SIGQUEUE
#	./$(CATCHER) SIGRT

clean:
	rm -f *.o

# EXAMPLE OF SENDER:
# ./sender 3245 1000 KILL
# ./sender 3245 1000 SIGQUEUE
# ./sender 3245 1000 SIGRT