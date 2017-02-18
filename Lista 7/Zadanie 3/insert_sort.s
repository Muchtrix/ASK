################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 3
################################################################################

    .text
    .globl insert_sort
    .type insert_sort, @function
insert_sort:                # %rdi - pierwszy element, %rsi - ostatni element
    mov %rdi, %r8
    sub $8, %rsi
petla1:
    cmp %r8, %rsi
    jz koniec
    add $8, %r8
    mov %r8, %r9
petla2:
    cmp %rdi, %r9
    jz petla1
    mov (%r9), %r10
    mov -8(%r9), %r11
    cmp %r10, %r11
    js petla1
    mov %r10, -8(%r9)
    mov %r11, (%r9)
    sub $8, %r9
    jmp petla2


koniec:
    ret

    .size insert_sort, .-insert_sort
