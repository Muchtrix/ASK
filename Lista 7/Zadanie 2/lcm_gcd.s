################################################################################
# Wiktor Adamski
# nr indeksu 272220
# Architektury systemów komputerowych - Lista 7 zadanie 2
################################################################################

    .text
    .globl lcm_gcd
    .type lcm_gcd, @function
lcm_gcd:
#    mov %rdx, %rdi
    cmp %rsi, %rdi          # %rsi - mniejsza z liczb
    jns gcd                  # %rdi - wieksza z liczb
    mov %rsi, %r8
    mov %rdi, %rsi
    mov %r8, %rdi
gcd:
    mov %rsi, %r10          # %r10 - aktualny kandydat na gcd
loop1:
    mov %rsi, %rax
    xor %rdx, %rdx          # (rdx:rax) = (0:rsi)
    div %r10                # (rdx:rax) = (rsi mod r10 : rsi div r10)
    test %rdx, %rdx
    jz gcd2
    dec %r10
    jmp loop1
gcd2:
    mov %rdi, %rax
    div %r10
    test %rdx, %rdx
    jz lcm
    dec %r10
    jmp loop1
lcm:
    mul %rsi                # %rax - lcm (z gcd*lcm = a*b)
    mov %r10, %rdx
    ret

    .size lcm_gcd, .-lcm_gcd
