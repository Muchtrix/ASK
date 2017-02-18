/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 4
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

unsigned long fibonacci(unsigned long x);

int main(int argc, char* argv[]){
    if (argc == 1) printf("Prosze podać numery liczb Fibonacciego do obliczenia.");
    for(int i = 1; i < argc; ++i){
        unsigned long liczba = atol(argv[i]);
        printf("%lu liczba Fibonacciego to: %lu\n", liczba, fibonacci(liczba));
    }
    return 0;
}
