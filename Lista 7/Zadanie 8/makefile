################################################################################
# Wiktor Adamski
# Nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 8
################################################################################

all: approx_sqrt.o test.o
	gcc -Wall -Wextra -o prog approx_sqrt.o test.o

approx_sqrt.o: approx_sqrt.s
	gcc -c -g approx_sqrt.s

test.o: test.c
	gcc -Og -c -g test.c

clean:
	rm -f test.o approx_sqrt.o prog
