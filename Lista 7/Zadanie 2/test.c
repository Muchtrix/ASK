/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 2
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

typedef struct {
unsigned long lcm, gcd;
} result_t;

result_t lcm_gcd(unsigned long, unsigned long);

int main(int argc, char* argv[]){
    if(argc >=3){
        unsigned long a, b;
        a = atol(argv[1]);
        b = atol(argv[2]);

        result_t wyn = lcm_gcd(a, b);
        printf("gcd: %lu, lcm: %lu\n", wyn.gcd, wyn.lcm);
    }
    else printf("Proszę podać 2 liczby do policzenia gcd i lcm.\n");
    return  0;
}
