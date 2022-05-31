CC = gcc
FLAGS = -Wall
LIB_NAME = lib_wc
SOURCE_NAME = library

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clean

zad2_static:
	make static
	$(CC) $(FLAGS) main.c -o test $(LIB_NAME).a -I .
	# works :)
	make tests

zad2_shared:
	make shared
	$(CC) $(FLAGS) main.c -o test -L. -I . -l_wc -Wl,-rpath=.
	# works :)
	make tests

zad2_dynamic:
	make shared
	$(CC) $(FLAGS) main.c -o test -I . -ldl -D dynamic
	# works :)
	make tests

tests:
	./test create_table 10 wc_files c.txt a.txt wc_files c.txt b.txt wc_files b.txt remove_block 1 delete_table
	./test create_table 100 wc_files c.txt e.txt wc_files d.txt b.txt a.txt wc_files b.txt remove_block 5 delete_table
	./test create_table 10 wc_files a.txt wc_files b.txt wc_files f.txt wc_files d.txt wc_files f.txt wc_files e.txt wc_files f.txt wc_files f.txt wc_files f.txt wc_files f.txt delete_table
	./test create_table 10 wc_files c.txt a.txt wc_files c.txt b.txt wc_files b.txt remove_block 1 remove_block 0 remove_block 2 delete_table
	./test create_table 10 wc_files c.txt a.txt remove_block 0 wc_files c.txt b.txt remove_block 0 wc_files b.txt remove_block 0 delete_table

clean:
	rm -f *.o