################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 7
################################################################################

WRITE  = 0x1
READ   = 0x0
EXIT   = 0x3c
ZMIANA = 0x20

    .lcomm ZNAK, 1

    .text
    .globl _start
    .type _start, @function
_start:
    mov $READ, %rax
    mov $0, %rdi
    mov $ZNAK, %rsi
    mov $1, %rdx
    syscall

    test %rax, %rax
    jz koniec
    mov $ZMIANA, %al
    cmpb $'z', (%rsi)
    ja pisz
    cmpb $'a', (%rsi)
    jb duze
    subb %al, (%rsi)
    jmp pisz
duze:
    cmpb $'A', (%rsi)
    jb pisz
    cmpb $'Z', (%rsi)
    ja pisz
    addb %al, (%rsi)
pisz:
    mov $WRITE, %rax
    mov $1, %rdi
    mov $1, %rdx
    syscall
    jmp _start
koniec:
    mov $EXIT, %rax
    mov $0, %rdi
    syscall

    .size _start, .-_start
