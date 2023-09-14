// Sent by btest_server to mark beginning of new function testing
// Immediately followed by an integer indicating which function we will test
// Constants for this integer defined at bottom of this file
#define NEW_FUNC_TAG 0

// Sent as reply from btest_driver to acknowledge start of new function tests
#define NEW_FUNC_ACK_TAG 1

// Sent to indicate that function inputs are coming from btest_server
// Followed by 0, 1, 2, or 3 integer values depending on current test subject (function)
#define ARGS_TAG 2

// Sent by btest_driver to indicate that return value from function coming next
// Immediately followed by a single integer value
#define FUNC_RETURN_TAG 3

// Sent by btest_server to indicate that we are done testing current function
// And that the function passed all tests
#define RESULT_SUCCESS_TAG 4

// Sent by btest_server to indicate that we are done testing current function
// And that the function has failed a test
// Immediately followed by an integer expressing expected function output
#define RESULT_FAILURE_TAG 5

// Sent by btest_driver to acknowledge receipt of function test outcome
// Maybe not a strictly necessary step but keeps two processes nicely in sync
#define RESULT_ACK_TAG 6

// Sent by btest_server to indicate an end to all tests
#define DONE_TAG 7

// Sent by btest_driver to indicate that a function timed out
#define TIMEOUT_TAG 8

// Sent by btest_driver to indicate that a function segfaulted
#define SEGFAULT_TAG 9

// Unique numbers to identify each function
#define BIT_AND_TAG 0
#define FITS_SHORT_TAG 1
#define ALL_EVEN_BITS_TAG 2
#define ANY_ODD_BIT_TAG 3
#define IS_EQUAL_TAG 4
#define FLOAT_IS_EQUAL_TAG 5
#define SIGN_TAG 6
#define IS_ASCII_DIGIT_TAG 7
#define FLOAT_IS_LESS_TAG 8
#define ROTATE_LEFT_TAG 9
#define ABS_VAL_TAG 10
#define FLOAT_SCALE_2_TAG 11
