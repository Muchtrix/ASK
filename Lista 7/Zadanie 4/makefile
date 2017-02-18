################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 4
################################################################################

all: fibonacci.o test.o
	gcc -Wall -Wextra fibonacci.o test.o -o prog

fibonacci.o: fibonacci.s
	gcc -c -g fibonacci.s

test.o: test.c
	gcc -Og -c -g test.c

clean:
	rm -f fibonacci.o test.o prog
