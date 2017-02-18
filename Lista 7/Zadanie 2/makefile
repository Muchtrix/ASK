################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 2
################################################################################

all: lcm_gcd.o test.o
	gcc -Wall -Wextra lcm_gcd.o test.o -o prog

lcm_gcd.o: lcm_gcd.s
	gcc -c -g lcm_gcd.s

test.o: test.c
	gcc -Og -c -g test.c

clean:
	rm -f lcm_gcd.o test.o prog
