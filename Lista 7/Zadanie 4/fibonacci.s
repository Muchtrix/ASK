################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 4
################################################################################

    .text
    .globl fibonacci
    .type fibonacci, @function

fibonacci:
    cmp $1, %rdi
    jz pierwszy
    cmp $0, %rdi
    jz zerowy
    push %r10
    dec %rdi
    push %rdi
    push %rbp
    call fibonacci
    pop %rbp
    pop %rdi
    mov %rax, %r10
    dec %rdi
    push %rdi
    push %rbp
    call fibonacci
    pop %rbp
    add %r10, %rax
    pop %rdi
    pop %r10
    ret

pierwszy:
    mov $1, %rax
    ret
zerowy:
    mov $0, %rax
    ret

    .size fibonacci, .-fibonacci
