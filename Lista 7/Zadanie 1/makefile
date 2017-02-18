################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 1
################################################################################

all: test.o clz.o
	gcc -Wall -Wextra test.o clz.o -o prog

test.o: test.c
	gcc -Og -Wall -Wextra -c test.c

clz.o: clz.s
	gcc -Wall -Wextra -c clz.s

clean:
	rm -f test.o clz.o prog
