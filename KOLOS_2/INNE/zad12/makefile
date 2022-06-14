PROG=main
LDLIBS=-pthread
FLAGS = -Wall

run:
	gcc $(FLAGS) ${PROG}.c -o ${PROG} $(LDLIBS)

test: ${PROG}
	@echo -e "\n\nTESTING\n\n"
	@./${PROG}

clean:
	rm ${PROG}

.PHONY: clean test