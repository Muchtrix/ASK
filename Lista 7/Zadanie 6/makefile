################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 6
################################################################################

all: prog.o
	gcc -Wall -Wextra prog.o -o prog

prog.o: prog.s
	gcc -c -g prog.s

clean:
	rm -f prog.o prog
