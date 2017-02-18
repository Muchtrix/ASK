################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 3
################################################################################

all: insert_sort.o test.o
	gcc -Wall -Wextra insert_sort.o test.o -o prog

insert_sort.o: insert_sort.s
	gcc -c -g insert_sort.s

test.o: test.c
	gcc -Og -c -g test.c

clean:
	rm -f insert_sort.o test.o prog
