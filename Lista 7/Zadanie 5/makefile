################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 5
################################################################################

all: mulf.o test.o
	gcc -Wall -Wextra mulf.o test.o -o prog

mulf.o: mulf.s
	gcc -c -g mulf.s

test.o: test.c
	gcc -Og -c -g test.c

clean:
	rm -f mnoz.o test.o prog
