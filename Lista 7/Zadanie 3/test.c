/******************************************************************************/
// Wiktor Adamski
// nr indeksu 272220
// Architektury systemów komputerowych - Lista 7 zadanie 3
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

long *tab;

void insert_sort(long*, long*);

int main(int argc, char* argv[]){
    if(argc == 1){
        printf("Proszę podać elementy tablicy do posortowania\n");
        return 0;
    }
    int ile = argc - 1;
    tab = (long *) malloc(sizeof(long)*ile);
    for(int i = 0; i < ile; ++i) tab[i] = atol(argv[i+1]);

    printf("Tablica: [");
    for(int i = 0; i < ile -1; ++i) printf("%ld, ", tab[i]);
    printf("%d]\n", tab[ile-1]);

    insert_sort(tab, tab+ile);

    printf("Po sortowaniu: [");
    for(int i = 0; i < ile; ++i) printf("%ld, ", tab[i]);
    printf("%d]\n", tab[ile-1]);

    return 0;
}
