################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 6
################################################################################

    .data
    .type napis, @object
napis:
    .asciz "Minimum: %ld, maksimum: %ld\n"
    .size napis, .-napis

    .text
    .globl main
    .type main, @function
main:
    lea (,%rdi, 8), %r8
    sub %r8, %rsp
    mov %rdi, %r8
    mov $1, %r9
petla1:
    cmp %r9, %r8
    jz po_atol
    mov (%rsi, %r9, 8), %rdi
    push %rsi
    push %r8
    push %r9
    call atol
    pop %r9
    pop %r8
    pop %rsi
    mov %rax, (%rsp, %r9, 8)
    inc %r9
    jmp petla1
po_atol:
    mov 8(%rsp), %r10
    mov %r10, %r11
    mov $2, %r9
petla2:
    cmp %r9, %r8
    jbe po_minMax
    cmp (%rsp, %r9, 8), %r10
    cmovs (%rsp, %r9, 8), %r10
    cmp (%rsp, %r9, 8), %r11
    cmovns (%rsp, %r9, 8), %r11
    inc %r9
    jmp petla2
po_minMax:
    mov $napis, %rdi
    mov %r11, %rsi
    mov %r10, %rdx
    push %r8
    xor %rax, %rax
    call printf
    pop %r8
    shl $3, %r8
    add %r8, %rsp
    xor %rdi, %rdi
    call exit

    .size main, .-main
