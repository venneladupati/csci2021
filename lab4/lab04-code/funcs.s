.text
.globl my_max
.globl my_pow

# Computes maximum of 2 arguments
# You can assume argument 1 (x) is in %edi
# You can assume argument 2 (y) is in %esi
# If you need to store temporary values, you may use the following registers:
#   %eax, %ecx, %edx, %esi, %edi, %r8d, %r9d, %r10d, %r11d
# DO NOT USE other registers. We will learn why soon.
/*
int my_max(int i, int j) {
    if (i > j) {
        return i;
    }
    return j;
}
*/

my_max:
    cmpl  %edi, %esi
    jl .L4
    movl %esi, %eax
    ret
.L4:
    movl %edi, %eax
    ret


# Computes base^exp
# You can assume argument 1 (base) is in %edi
# You can assume argument 2 (exp) is in %esi
# If you need to store temporary values, you may use the following registers:
#   %eax, %ecx, %edx, %esi, %edi, %r8d, %r9d, %r10d, %r11d
# DO NOT USE other registers. We will learn why soon.
/*
int my_pow(int base, int exp) {
    int result = base;
    for (int i = 2; i <= exp; i++) {
        result *= base;
    }
    return result;
}
*/

my_pow:
    movl %edi, %eax
    movl $2, %ecx
    jmp LOOP
LOOP:
    cmpl %esi, %ecx
    jg END
    imull %edi, %eax
    addl $1, %ecx
    jmp LOOP
END:
    ret
    

