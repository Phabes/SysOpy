FLAGS = -Wall

run:
	gcc $(FLAGS) -c main.c
	gcc $(FLAGS) main.o -o main
	make clean
	make test_all

test_all:
	./main ./ bbba 5
	./main ./ plll xf
	./main ../svd plll 7
	./main ../stat_all.c plll 1
	./main ../../lab3/zad3 nksdcskjnvoas 1
	./main ./ nksdcskjnvoas 2

clean:
	rm -f *.o