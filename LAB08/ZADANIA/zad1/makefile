FLAGS = -Wall

run:
	gcc $(FLAGS) main.c -o main -lpthread
	make clean

tests:
	./main 1 numbers images/baboon.ascii.pgm negatives/baboon.ascii.pgm
	./main 1 block images/baboon.ascii.pgm negatives/baboon.ascii_2.pgm
	./main 2 numbers images/balloons.ascii.pgm negatives/balloons.ascii.pgm
	./main 2 block images/balloons.ascii.pgm negatives/balloons.ascii_2.pgm
	./main 4 numbers images/x31_f18.ascii.pgm negatives/x31_f18.ascii.pgm
	./main 4 block images/x31_f18.ascii.pgm negatives/x31_f18.ascii_2.pgm
	./main 8 numbers images/pepper.ascii.pgm negatives/pepper.ascii.pgm
	./main 8 block images/pepper.ascii.pgm negatives/pepper.ascii_2.pgm
	./main 16 numbers images/mountain.ascii.pgm negatives/mountain.ascii.pgm
	./main 16 block images/mountain.ascii.pgm negatives/mountain.ascii_2.pgm
	./main 25 numbers images/mona_lisa.ascii.pgm negatives/mona_lisa.ascii.pgm
	./main 25 block images/mona_lisa.ascii.pgm negatives/mona_lisa.ascii_2.pgm

clean:
	rm -f *.o