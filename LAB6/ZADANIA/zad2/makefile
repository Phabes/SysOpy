FLAGS = -Wall
SERVER = server
CLIENT = client

run:
	gcc $(FLAGS) -c $(SERVER).c
	gcc $(FLAGS) $(SERVER).o -o $(SERVER) -lrt
	gcc $(FLAGS) -c $(CLIENT).c
	gcc $(FLAGS) $(CLIENT).o -o $(CLIENT) -lrt
	make clean

clean:
	rm -f *.o