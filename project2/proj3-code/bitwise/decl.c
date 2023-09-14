// DO NOT MODIFY THIS FILE
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TMin INT_MIN
#define TMax INT_MAX

#include "btest.h"
#include "bits_test.h"
#include "message_tags.h"

test_rec test_set[] = {
 {"bitAnd", (funct_t) test_bitAnd, BIT_AND_TAG, 2,
    "~ |", 8, 1,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"fitsShort", (funct_t) test_fitsShort, FITS_SHORT_TAG, 1,
    "! ~ & ^ | + << >>", 8, 1,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
{"allEvenBits", (funct_t) test_allEvenBits, ALL_EVEN_BITS_TAG, 1,
     "! ~ & ^ | + << >>", 12, 2,
    {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"anyOddBit", (funct_t) test_anyOddBit, ANY_ODD_BIT_TAG, 1,
    "! ~ & ^ | + << >>", 12, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"isEqual", (funct_t) test_isEqual, IS_EQUAL_TAG,2,
    "! ~ & ^ | + << >>", 5, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"floatIsEqual", (funct_t) test_floatIsEqual, FLOAT_IS_EQUAL_TAG,2,
    "$", 25, 2,
     {{-1, -1},{-1,-1},{-1,-1}}},
 {"sign", (funct_t) test_sign, SIGN_TAG,1,
    "! ~ & ^ | + << >>", 15, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"isAsciiDigit", (funct_t) test_isAsciiDigit, IS_ASCII_DIGIT_TAG, 1,
    "! ~ & ^ | + << >>", 15, 3,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"floatIsLess", (funct_t) test_floatIsLess, FLOAT_IS_LESS_TAG, 2,
    "$", 30, 3,
     {{-1, -1},{-1,-1},{-1,-1}}},
 {"rotateLeft", (funct_t) test_rotateLeft, ROTATE_LEFT_TAG, 2,
     "! ~ & ^ | + << >>", 25, 3,
  {{TMin, TMax},{0,31},{TMin,TMax}}},
 {"absVal", (funct_t) test_absVal, ABS_VAL_TAG, 1,
 "! ~ & ^ | + << >>", 10, 4, {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
{"floatScale2", (funct_t) test_floatScale2, FLOAT_SCALE_2_TAG, 1,
    "$", 30, 4,
    {{1, 1}, {1,1}, {1,1}}},
 {"", NULL, 0, 0, "", 0, 0, {{0,0}, {0,0}, {0,0}}} // Apparently we need this as a sentinel value
};
// DO NOT MODIFY THIS FILE
