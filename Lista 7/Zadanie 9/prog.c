/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 9
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char* argv[]){
    if(argc > 2){
        long a = atol(argv[1]);
        long b = atol(argv[2]);
        long wynik;
        asm("mov %1, %0;\
             mov $0x80000000000000, %%r8;\
             mov $0x7fffffffffffff, %%r9;\
             add %2, %0;\
             cmovs %%r9, %%r8;\
             cmovo %%r8, %0"
             : "=r"(wynik)
             : "r"(a), "r"(b)
             : "%r8", "%r9", "cc" );

        printf("%ld + %ld = %ld\n", a, b, wynik);
    } else printf("Proszę podać 2 liczby do dodania\n");
    return 0;
}
