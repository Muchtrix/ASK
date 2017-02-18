/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 5
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

typedef union{
    unsigned calk;
    float zmien;
} liczba;

unsigned mnoz(unsigned x, unsigned y);

int main(int argc, char* argv[]){
    if(argc > 2){
        liczba a, b, wyn;
        a.zmien = atof(argv[1]);
        b.zmien = atof(argv[2]);
        wyn.calk = mulf(a.calk, b.calk);
        printf("%f * %f = %f\n", a.zmien, b.zmien, wyn.zmien);
    } else printf("Proszę podać 2 liczby do wymnożenia\n");

    return 0;
}
