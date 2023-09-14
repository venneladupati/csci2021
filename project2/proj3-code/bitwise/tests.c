/* Testing Code */

#include <limits.h>
#include <math.h>

/* Routines used by floation point test code */

/* Convert from bit level representation to floating point number */
float u2f(unsigned u) {
    union {
        unsigned u;
        float f;
    } a;
    a.u = u;
    return a.f;
}

/* Convert from floating point number to bit-level representation */
unsigned f2u(float f) {
    union {
        unsigned u;
        float f;
    } a;
    a.f = f;
    return a.u;
}

int test_bitAnd(int x, int y) {
    return x&y;
}

int test_fitsShort(int x) {
    short int sx = (short int) x;
    return x == sx;
}
int test_allEvenBits(int x) {
    int i;
    for (i = 0; i < 32; i+=2) {
        if ((x & (1<<i)) == 0) {
            return 0;
        }
    }
    return 1;
}

int test_anyOddBit(int x) {
    int i;
    for (i = 1; i < 32; i+=2) {
        if (x & (1<<i)) {
            return 1;
        }
    }
    return 0;
}

int test_isEqual(int x, int y) {
    return x == y;
}

int test_floatIsEqual(unsigned uf, unsigned ug) {
    float f = u2f(uf);
    float g = u2f(ug);
    return f == g;
}

int test_sign(int x) {
    if (!x) {
        return 0;
    }
    return (x < 0) ? -1 : 1;
}

int test_isAsciiDigit(int x) {
    return (0x30 <= x) && (x <= 0x39);
}

int test_floatIsLess(unsigned uf, unsigned ug) {
    float f = u2f(uf);
    float g = u2f(ug);
    return f < g;
}

int test_rotateLeft(int x, int n) {
    unsigned u = (unsigned) x;
    int i;
    for (i = 0; i < n; i++) {
        unsigned msb = u >> 31;
        unsigned rest = u << 1;
        u = rest | msb;
    }
    return (int) u;
}

int test_absVal(int x) {
    return (x < 0) ? -x : x;
}

unsigned test_floatScale2(unsigned uf) {
    float f = u2f(uf);
    float tf = 2*f;
    if (isnan(f)) {
        return uf;
    }
    else {
        return f2u(tf);
    }
}
