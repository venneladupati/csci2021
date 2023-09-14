# Read the following instructions carefully.
# You will complete your solution to the bitwise portion of the project by
# editing the collection of functions in this source file.
#
# Some rules from Project 2 are still in effect for your assembly code here:
#  1. No global variables are allowed
#  2. You may not define or call any additional functions in this file.
#  3. You may not use any floating-point assembly operations.
#
# You may assume that your machine:
#  1. Uses two's complement, 32-bit representations of integers

# Use the 'btest_driver' program to check your functions for correctness

.text

# bitAnd - Compute x & y (bitwise AND of the two arguments)
#   Rating: 1
.global bitAnd
bitAnd:
    movl $2, %eax
    ret


# fitsShort - return 1 if the single argument x can be represented as a 16-bit,
# two's complement integer.
#   Rating: 1
.global fitsShort
fitsShort:
    movl $2, %eax
    ret


# allEvenBits - return 1 if all even-numbered bits in word set to 1
#   where bits are numbered from 0 (least significant) to 31 (most significant)
#   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
#   Rating: 2
.global allEvenBits
allEvenBits:
    movl $2, %eax
    ret


# anyOddBit - return 1 if any odd-numbered bit in word set to 1
#   where bits are numbered from 0 (least significant) to 31 (most significant)
#   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
#   Rating: 2
.global anyOddBit
anyOddBit:
    movl $2, %eax
    ret


# isEqual - return 1 if x == y, and 0 otherwise
#   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
#   Rating: 2
.global isEqual
isEqual:
    movl $2, %eax
    ret


# floatIsEqual - Compute f == g for floating point arguments f and g.
#   Both the arguments are passed as unsigned int's, but
#   they are to be interpreted as the bit-level representations of
#   single-precision floating point values.
#   If either argument is NaN, return 0.
#   +0 and -0 are considered equal.
#   Rating: 2
.global floatIsEqual
floatIsEqual:
    movl $2, %eax
    ret


# sign - return 1 if positive, 0 if zero, and -1 if negative
#  Examples: sign(130) = 1
#            sign(-23) = -1
#  Rating: 2
.global sign
sign:
    movl $2, %eax
    ret


# isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
#   Example: isAsciiDigit(0x35) = 1.
#            isAsciiDigit(0x3a) = 0.
#            isAsciiDigit(0x05) = 0.
#   Rating: 3
.global isAsciiDigit
isAsciiDigit:
    movl $2, %eax
    ret


# floatIsLess - Compute f < g for floating point arguments f and g.
#   Both the arguments are passed as unsigned int's, but
#   they are to be interpreted as the bit-level representations of
#   single-precision floating point values.
#   If either argument is NaN, return 0.
#   +0 and -0 are considered equal.
#   Rating: 3
.global floatIsLess
floatIsLess:
    movl $2, %eax
    ret


# rotateLeft - Rotate x to the left by n
#   Can assume that 0 <= n <= 31
#   Examples: rotateLeft(0x87654321,4) = 0x76543218
#   Rating: 3
.global rotateLeft
rotateLeft:
    movl $2, %eax
    ret


# absVal - absolute value of x
#   Example: absVal(-1) = 1.
#   You may assume -TMax <= x <= TMax
#   Rating: 4
.global absVal
absVal:
    movl $2, %eax
    ret


# floatScale2 - Return bit-level equivalent of expression 2*f for
#   floating point argument f.
#   Both the argument and result are passed as unsigned int's, but
#   they are to be interpreted as the bit-level representation of
#   single-precision floating point values.
#   When argument is NaN, return argument
#   Rating: 4
.global floatScale2
floatScale2:
    movl $2, %eax
    ret
