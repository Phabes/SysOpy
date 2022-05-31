FLAGS = -Wall
SERVER = server
CLIENT = client

run:
	gcc $(FLAGS) -c $(SERVER).c
	gcc $(FLAGS) $(SERVER).o -o $(SERVER)
	gcc $(FLAGS) -c $(CLIENT).c
	gcc $(FLAGS) $(CLIENT).o -o $(CLIENT)
	make clean

clean:
	rm -f *.o