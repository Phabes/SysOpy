PROG=main
FLAGS=-Wall

all:
	gcc $(FLAGS) ${PROG}.c -o ${PROG}

test: ${PROG}
	@echo -e "\n\nTESTING\n\n"
	./${PROG}

clean:
	rm ${PROG}

.PHONY: clean test
