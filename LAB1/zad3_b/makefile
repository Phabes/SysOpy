CC = gcc
FLAGS = -Wall
LIB_NAME = lib_wc
SOURCE_NAME = library
O = O0
RAPORT = raport3b.txt

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clean

zad3_static:
	make static
	$(CC) $(FLAGS) main.c -o test -$(O) $(LIB_NAME).a -I .
	# works :)
	make tests

zad3_shared:
	make shared
	$(CC) $(FLAGS) main.c -o test -L. -$(O) -I . -l_wc -Wl,-rpath=.
	# works :)
	make tests

zad3_dynamic:
	make shared
	$(CC) $(FLAGS) main.c -o test -$(O) -I . -ldl -D dynamic
	# works :)
	make tests

tests:
	./test create_table 10 wc_files c.txt a.txt wc_files c.txt b.txt wc_files b.txt remove_block 1 delete_table
	./test create_table 100 wc_files c.txt e.txt wc_files d.txt b.txt a.txt wc_files b.txt remove_block 5 delete_table
	./test create_table 10 wc_files a.txt wc_files b.txt wc_files f.txt wc_files d.txt wc_files f.txt wc_files e.txt wc_files f.txt wc_files f.txt wc_files f.txt wc_files f.txt delete_table
	./test create_table 10 wc_files c.txt a.txt wc_files c.txt b.txt wc_files b.txt remove_block 1 remove_block 0 remove_block 2 delete_table
	./test create_table 10 wc_files c.txt a.txt remove_block 0 wc_files c.txt b.txt remove_block 0 wc_files b.txt remove_block 0 delete_table

test_all:
	printf "static O0:\n" >> $(RAPORT)
	make zad3_static O=O0
	printf "static O1:\n" >> $(RAPORT)
	make zad3_static O=O1
	printf "static O2:\n" >> $(RAPORT)
	make zad3_static O=O2
	printf "static O3:\n" >> $(RAPORT)
	make zad3_static O=O3

	printf "shared O0:\n" >> $(RAPORT)
	make zad3_shared O=O0
	printf "shared O1:\n" >> $(RAPORT)
	make zad3_shared O=O1
	printf "shared O2:\n" >> $(RAPORT)
	make zad3_shared O=O2
	printf "shared O3:\n" >> $(RAPORT)
	make zad3_shared O=O3

	printf "dynamic O0:\n" >> $(RAPORT)
	make zad3_dynamic O=O0
	printf "dynamic O1:\n" >> $(RAPORT)
	make zad3_dynamic O=O1
	printf "dynamic O2:\n" >> $(RAPORT)
	make zad3_dynamic O=O2
	printf "dynamic O3:\n" >> $(RAPORT)
	make zad3_dynamic O=O3


clean:
	rm -f *.o