FLAGS = -Wall
NFTW = nftw_all
STAT = stat_all

run_stat:
	gcc $(FLAGS) -c $(STAT).c
	gcc $(FLAGS) $(STAT).o -o $(STAT)
	make clean
	make test_stat

run_nftw:
	gcc $(FLAGS) -c $(NFTW).c
	gcc $(FLAGS) $(NFTW).o -o $(NFTW)
	make clean
	make test_nftw

test_stat:
	./$(STAT) /home/wojtek/Pulpit/SysOpy/lab2/zad3
	./$(STAT) /home
	./$(STAT) ..
	./$(STAT) /none

test_nftw:
	./$(NFTW) /home/wojtek/Pulpit/SysOpy/lab2/zad3
#	./$(NFTW) /home
#	./$(NFTW) ..
#	./$(NFTW) /none

test:
	make run_stat
	make run_nftw

clean:
	rm -f *.o