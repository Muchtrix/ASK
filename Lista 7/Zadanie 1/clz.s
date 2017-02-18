################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 1
################################################################################

    .data
    .align 8
    .type MASKA, @object
    .size MASKA, 8
MASKA:
    .quad 0xffffffff

    .type DL_MASKI, @object
    .size DL_MASKI, 4
DL_MASKI:
    .long 32

    .text
    .globl  clz
    .type   clz, @function


clz:
    test %rdi, %rdi
    jnz prog
    mov $64, %rax
    ret
prog:
    mov $0x0, %rax
    mov MASKA, %rsi              # %rsi  to aktualna maska
    mov DL_MASKI, %r9            # %r9  to długość maski
    mov DL_MASKI, %r10           # %r10 to aktualne przesunięcie
petla:
    movq %rsi, %r8               # %r8 to maska po przesunięciu
    movq %r10, %rcx
    shl %cl, %r8
    shr %r9
    movq %r9, %rcx
    shr %cl, %rsi
    and %rdi, %r8
    jz zerowy

    add %r9, %r10
    jmp war
zerowy:
    lea (%rax, %r9, 2), %rax
    sub %r9, %r10
war:
    test %r9, %r9
    jz post
    jmp petla
post:
    movq $1, %r8
    movq %r10, %rcx
    shl %cl, %r8
    and %rdi, %r8
    jnz koniec
    inc %rax
koniec:
    ret

    .size clz, .-clz
