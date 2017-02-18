################################################################################
# Wiktor Adamski
# Nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 8
################################################################################

    .section .rodata
    .type POL, @object
    .size POL, 8
POL:
    .double 0.5
    .type ABS, @object
    .size ABS, 8
ABS:
    .double NaN


    .text
    .globl approx_sqrt
    .type approx_sqrt, @function
approx_sqrt:
    movsd POL, %xmm2
    movsd ABS, %xmm5
    movsd %xmm0, %xmm3
    mulsd %xmm2, %xmm3
    ucomisd %xmm1, %xmm3
    jbe KONIEC
PETLA:
    movsd %xmm0, %xmm4
    divsd %xmm3, %xmm4
    addsd %xmm3, %xmm4
    mulsd %xmm2, %xmm4         # %xmm4 = (x + a/x)/2
    subsd %xmm4, %xmm3
    andpd %xmm5, %xmm3
    ucomisd %xmm1, %xmm3
    jbe PREP
    movsd %xmm4, %xmm3
    jmp PETLA
PREP:
    movsd %xmm4, %xmm3
KONIEC:
    movsd %xmm3, %xmm0
    ret
    .size approx_sqrt, .-approx_sqrt
