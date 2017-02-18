/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 1
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

long clz(long);

void bity(long);

int main(int argc, char* argv[]){
    if(argc == 1){
        printf("Proszę podać liczby do sprawdzenia długości prefiksu\n");
    }
    for(int i = 1; i < argc; ++i){
        long liczba = atol(argv[i]);
        long ile = clz(liczba);
        printf("Liczba %ld ma prefix długości %ld\nBity:", liczba, ile);
        bity(liczba);
        printf("\n");
    }
    return 0;
}

void bity(long x){
    for(int i = 63; i >=0 ; --i){
        int b = (x & (1l<<i))? 1 : 0;
        if(i%8 == 7) putchar(' ');
        printf("%d", b);
    }
}
