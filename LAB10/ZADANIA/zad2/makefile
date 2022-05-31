FLAGS = -Wall

run:
	make remove
	gcc $(FLAGS) server.c -o server -lpthread
	gcc $(FLAGS) client.c -o client -lpthread
	make clean

server1:
	./server 7001 socket

client1:
	./client Wojtek unix socket

client2:
	./client Kazek unix socket

client1_web:
	./client Jozek inet 7001

client2_web:
	./client Pioter inet 7001

clean:
	rm -f *.o

remove:
	rm -f socket 1* 2* 3* 4* 5* 6* 7* 8* 9*