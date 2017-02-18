################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 5
################################################################################

    .data
    .type ZNAK, @object
    .size ZNAK, 4
ZNAK:
    .long 0x80000000

    .type CECHA, @object
    .size CECHA, 4
CECHA:
    .long 0x7f800000

    .type WYKL, @object
    .size WYKL, 4
WYKL:
    .long 0x007fffff

    .type JEDYNKA, @object
    .size JEDYNKA, 4
JEDYNKA:
    .long 0x00800000

    .type NORM, @object
    .size NORM, 4
NORM:
    .long 0x01000000

    .type DOD, @object
    .size DOD, 4
DOD:
    .long 0x00008000

    .text
    .globl mulf
    .type mulf, @function
mulf:
    push %rbx
    push %rcx
    mov %edi, %ebx              # Wyliczenie cechy wyniku
    and CECHA, %ebx
    shr $23, %ebx
    mov %esi, %ecx
    and CECHA, %ecx
    shr $23, %ecx
    add %ecx, %ebx
    sub $127, %ebx              # %ebx - cecha

    mov %edi, %ecx              # Wyliczenie znaku wyniku
    xor %esi, %ecx
    and ZNAK, %ecx              # %ecx - znaku

    and WYKL, %edi
    and WYKL, %esi
    or JEDYNKA, %edi
    or JEDYNKA, %esi
    mov %edi, %eax
    xor %edx, %edx
    mul %esi                    #  edx:eax - iloczyn

    test DOD, %edx
    jz norm1
    inc %ebx
norm1:
    test %edx, %edx
    jz norm2
    shr %edx
    rcr %eax
    jmp norm1
norm2:
    cmp NORM, %eax
    jbe koniec
    shr %eax
    jmp norm2
koniec:
    and WYKL, %eax
    shl $23, %ebx
    or %ebx, %eax
    or %ecx, %eax
    pop %rcx
    pop %rbx
    ret
    .size mulf, .-mulf
