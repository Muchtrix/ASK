/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 8
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

double approx_sqrt(double x, double eps);

int main(int argc, char* argv[]){
    if(argc > 2){
        double x = strtod(argv[1], NULL);
        double eps = strtod(argv[2], NULL);
        double abc = approx_sqrt(x, eps);
        printf("%f\n", abc);
    } else printf("Proszę podać x i epsilon.\n");
    return 0;
}
