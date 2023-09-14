/*
 * btest.c - A test harness that checks a student's solution in bits.c
 *           for correctness.
 *
 * Copyright (c) 2001-2011, R. Bryant and D. O'Hallaron, All rights
 * reserved.  May not be used, modified, or copied without permission.
 *
 * This is an improved version of btest that tests large windows
 * around zero and tmin and tmax for integer puzzles, and zero, norm,
 * and denorm boundaries for floating point puzzles.
 *
 * Note: not 64-bit safe. Always compile with gcc -m32 option.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "btest.h"
#include "message_tags.h"

/* Not declared in some stdlib.h files, so define here */
float strtof(const char *nptr, char **endptr);

/*************************
 * Configuration Constants
 *************************/

/* For functions with a single argument, generate TEST_RANGE values
   above and below the min and max test values, and above and below
   zero. Functions with two or three args will use square and cube
   roots of this value, respectively, to avoid combinatorial
   explosion */
#define TEST_RANGE 200000

/* This defines the maximum size of any test value array. The
   gen_vals() routine creates k test values for each value of
   TEST_RANGE, thus MAX_TEST_VALS must be at least k*TEST_RANGE */
#define MAX_TEST_VALS 12*TEST_RANGE

/**********************************
 * Globals defined in other modules
 **********************************/
/* This characterizes the set of puzzles to test.
   Defined in decl.c and generated from templates in ./puzzles dir */
extern test_rec test_set[];

/************************************************
 * Write-once globals defined by command line args
 ************************************************/

/* If non-NULL, test only one function (-f) */
static char* test_fname = NULL;

/* Special case when only use fixed argument(s) (-1, -2, or -3) */
static int has_arg[3] = {0,0,0};
static unsigned argval[3] = {0,0,0};

/* Use fixed weight for rating, and if so, what should it  be? (-r) */
static int global_rating = 0;

/******************
 * Helper functions
 ******************/
/*
 * random_val - Return random integer value between min and max
 */
static int random_val(int min, int max) {
    double weight = rand()/(double) RAND_MAX;
    int result = min * (1-weight) + max * weight;
    return result;
}

/*
 * gen_vals - Generate the integer values we'll use to test a function
 */
static int gen_vals(int test_vals[], int min, int max, int test_range, int arg) {
    int i;
    int test_count = 0;

    /* Special case: If the user has specified a specific function
       argument using the -1, -2, or -3 flags, then simply use this
       argument and return */
    if (has_arg[arg]) {
        test_vals[0] = argval[arg];
        return 1;
    }

    /*
     * Special case: Generate test vals for floating point functions
     * where the input argument is an unsigned bit-level
     * representation of a float. For this case we want to test the
     * regions around zero, the smallest normalized and largest
     * denormalized numbers, one, and the largest normalized number,
     * as well as inf and nan.
     */

    // Hack for the floatIsLess Case, mark it with range {-1,-1}
    // It's so slow that we need to drastically reduce test range
    if ((min == 1 && max == 1) || (min == -1 && max == -1)) {
        // Reduce floating point test range so these tests run in a reasonable amount of time
        if (min == -1 && max == -1) {
            test_range = 100;
        } else {
            test_range = 100000;
        }

        unsigned smallest_norm = 0x00800000;
        unsigned one = 0x3f800000;
        unsigned largest_norm = 0x7f000000;

        unsigned inf = 0x7f800000;
        unsigned nan =  0x7fc00000;
        unsigned sign = 0x80000000;

        /* Test range should be at most 1/2 the range of one exponent
           value */
        if (test_range > (1 << 23)) {
            test_range = 1 << 23;
        }

        /* Functions where the input argument is an unsigned bit-level
           representation of a float. The number of tests generated
           inside this loop body is the value k referenced in the
           comment for the global variable MAX_TEST_VALS. */

        for (i = 0; i < test_range; i++) {
            /* Denorms around zero */
            test_vals[test_count++] = i;
            test_vals[test_count++] = sign | i;

            /* Region around norm to denorm transition */
            test_vals[test_count++] = smallest_norm + i;
            test_vals[test_count++] = smallest_norm - i;
            test_vals[test_count++] = sign | (smallest_norm + i);
            test_vals[test_count++] = sign | (smallest_norm - i);

            /* Region around one */
            test_vals[test_count++] = one + i;
            test_vals[test_count++] = one - i;
            test_vals[test_count++] = sign | (one + i);
            test_vals[test_count++] = sign | (one - i);

            /* Region below largest norm */
            test_vals[test_count++] = largest_norm - i;
            test_vals[test_count++] = sign | (largest_norm - i);
        }

        /* special vals */
        test_vals[test_count++] = inf;        /* inf */
        test_vals[test_count++] = sign | inf; /* -inf */
        test_vals[test_count++] = nan;        /* nan */
        test_vals[test_count++] = sign | nan; /* -nan */

        return test_count;
    }


    /*
     * Normal case: Generate test vals for integer functions
     */

    /* If the range is small enough, then do exhaustively */
    if (max - MAX_TEST_VALS <= min) {
        for (i = min; i <= max; i++)
            test_vals[test_count++] = i;
        return test_count;
    }

    /* Otherwise, need to sample.  Do so near the boundaries, around
       zero, and for some random cases. */
    for (i = 0; i < test_range; i++) {

    /* Test around the boundaries */
    test_vals[test_count++] = min + i;
    test_vals[test_count++] = max - i;

    /* If zero falls between min and max, then also test around zero */
    if (i >= min && i <= max)
        test_vals[test_count++] = i;
    if (-i >= min && -i <= max)
        test_vals[test_count++] = -i;

    /* Random case between min and max */
    test_vals[test_count++] = random_val(min, max);

    }
    return test_count;
}

/*
 * test_0_arg - Test a function with zero arguments
 */
static int test_0_arg(int student_result, funct_t ft, char *name, int *expected_result) {
    *expected_result = ft();
    int error =  (student_result != *expected_result);
    return error;
}

/*
 * test_1_arg - Test a function with one argument
 */
static int test_1_arg(int student_result, funct_t ft, int arg1, char *name, int *expected_result) {
    funct1_t f1t = (funct1_t) ft;

    *expected_result = f1t(arg1);
    int error = (student_result != *expected_result);
    return error;
}

/*
 * test_2_arg - Test a function with two arguments
 */
static int test_2_arg(int student_result, funct_t ft, int arg1, int arg2, char *name, int *expected_result) {
    funct2_t f2t = (funct2_t) ft;
    *expected_result = f2t(arg1, arg2);
    int error = (student_result != *expected_result);
    return error;
}

/*
 * test_3_arg - Test a function with three arguments
 */
static int test_3_arg(int student_result, funct_t ft, int arg1, int arg2, int arg3, char *name, int *expected_result) {
    funct3_t f3t = (funct3_t) ft;
    *expected_result = f3t(arg1, arg2, arg3);
    int error = (student_result != *expected_result);
    return error;
}

/*
 * test_function - Test a function.  Return number of 1 if timeout or segfault occurred, 0 otherwise
 */
static int test_function(test_ptr t) {
    int test_counts[3];         /* number of test values for each arg */
    int args = t->args;         /* number of function arguments */
    int arg_test_range[3] = {}; /* test range for each argument */
    int i, a1, a2, a3;
    int errors = 0;
    int expected_result;

    /* These are the test values for each arg. Declared with the
       static attribute so that the array will be allocated in bss
       rather than the stack */
    static int arg_test_vals[3][MAX_TEST_VALS];

    /* Assign range of argument test vals so as to conserve the total
       number of tests, independent of the number of arguments */
    if (args == 1) {
        arg_test_range[0] = TEST_RANGE;
    }
    else if (args == 2) {
        arg_test_range[0] = pow((double)TEST_RANGE, 0.5);  /* sqrt */
        arg_test_range[1] = arg_test_range[0];
    }
    else {
        arg_test_range[0] = pow((double)TEST_RANGE, 0.333); /* cbrt */
        arg_test_range[1] = arg_test_range[0];
        arg_test_range[2] = arg_test_range[0];
    }

    /* Sanity check on the ranges */
    if (arg_test_range[0] < 1)
        arg_test_range[0] = 1;
    if (arg_test_range[1] < 1)
        arg_test_range[1] = 1;
    if (arg_test_range[2] < 1)
        arg_test_range[2] = 1;

    /* Create a test set for each argument */
    for (i = 0; i < args; i++) {
        test_counts[i] =  gen_vals(arg_test_vals[i],
                   t->arg_ranges[i][0], /* min */
                   t->arg_ranges[i][1], /* max */
                   arg_test_range[i],
                   i);

    }

    // Announce start of new function to driver
    int message_tag = NEW_FUNC_TAG;
    if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
            write(STDOUT_FILENO, &t->function_id, sizeof(int)) == -1 ||
            write(STDOUT_FILENO, &t->rating, sizeof(int)) == -1) {
        perror("write");
        exit(1);
    }

    // Await acknowledgement from driver
    if (read(STDIN_FILENO, &message_tag, sizeof(int)) == -1) {
        perror("read");
        exit(1);
    } else if (message_tag != NEW_FUNC_ACK_TAG) {
        fprintf(stderr, "Did not receive proper NEW_FUNC_ACK from driver, got %d instead\n", message_tag);
        exit(1);
    }

    /* Test function has no arguments */
    if (args == 0) {
        // Ask btest_driver to run function
        message_tag = ARGS_TAG;
        if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1) {
            perror("write");
            exit(1);
        }

        // Read student result from btest_driver
        if (read(STDIN_FILENO, &message_tag, sizeof(int)) == -1) {
            perror("read");
            exit(1);
        }

        if (message_tag == TIMEOUT_TAG || message_tag == SEGFAULT_TAG) {
            // Give up, function has timed out and/or failed
            return 1;
        } else if (message_tag != FUNC_RETURN_TAG) {
            fprintf(stderr, "Did not receive valid FUNC_RETURN_TAG from driver\n");
            exit(1);
        }

        int student_result;
        if (read(STDIN_FILENO, &student_result, sizeof(int)) == -1) {
            perror("read");
            exit(1);
        }

        errors = test_0_arg(student_result, t->test_funct, t->name, &expected_result);
        if (errors) {
            message_tag = RESULT_FAILURE_TAG;
            if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                    write(STDOUT_FILENO, &expected_result, sizeof(int)) == -1) {
                perror("write");
                exit(1);
            }
            return 0;
        }
    } else if (args == 1) {
        for (a1 = 0; a1 < test_counts[0]; a1++) {
            int arg1 = arg_test_vals[0][a1];
            // Ask btest_driver to run function
            message_tag = ARGS_TAG;
            if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                    write(STDOUT_FILENO, &arg1, sizeof(int)) == -1) {
                perror("write");
                exit(1);
            }

            // Read student result from btest_driver
            if (read(STDIN_FILENO, &message_tag, sizeof(int)) == -1) {
                perror("read");
                exit(1);
            }

            if (message_tag == TIMEOUT_TAG || message_tag == SEGFAULT_TAG) {
                // Give up, function has timed out and/or failed
                return 1;
            } else if (message_tag != FUNC_RETURN_TAG) {
                fprintf(stderr, "Did not receive valid FUNC_RETURN_TAG from driver\n");
                exit(1);
            }

            int student_result;
            if (read(STDIN_FILENO, &student_result, sizeof(int)) == -1) {
                perror("read");
                exit(1);
            }

            errors = test_1_arg(student_result,
                     t->test_funct,
                     arg1,
                     t->name, &expected_result);

            /* Stop testing if there is an error */
            if (errors) {
                message_tag = RESULT_FAILURE_TAG;
                if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                        write(STDOUT_FILENO, &expected_result, sizeof(int)) == -1) {
                    perror("write");
                    exit(1);
                }
                return 0;
            }
        }
    } else if (args == 2) {
        for (a1 = 0; a1 < test_counts[0]; a1++) {
            int arg1 = arg_test_vals[0][a1];
            for (a2 = 0; a2 < test_counts[1]; a2++) {
                int arg2 = arg_test_vals[1][a2];
                // Ask btest_driver to run function
                message_tag = ARGS_TAG;
                if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                        write(STDOUT_FILENO, &arg1, sizeof(int)) == -1 ||
                        write(STDOUT_FILENO, &arg2, sizeof(int)) == -1) {
                    perror("write");
                    exit(1);
                }

                // Read student result from btest_driver
                if (read(STDIN_FILENO, &message_tag, sizeof(int)) == -1) {
                    perror("read");
                    exit(1);
                }

                if (message_tag == TIMEOUT_TAG || message_tag == SEGFAULT_TAG) {
                    // Give up, function has timed out and/or failed
                    return 1;
                } else if (message_tag != FUNC_RETURN_TAG) {
                    fprintf(stderr, "Did not receive valid FUNC_RETURN_TAG from driver\n");
                    exit(1);
                }

                int student_result;
                if (read(STDIN_FILENO, &student_result, sizeof(int)) == -1) {
                    perror("read");
                    exit(1);
                }

                errors = test_2_arg(student_result,
                        t->test_funct,
                        arg1,
                        arg2,
                        t->name, &expected_result);
                /* Stop testing if there is an error */
                if (errors) {
                    message_tag = RESULT_FAILURE_TAG;
                    if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                            write(STDOUT_FILENO, &expected_result, sizeof(int)) == -1) {
                        perror("write");
                        exit(1);
                    }
                    return 0;
                }
            }
        }
    } else if (args == 3) {
        for (a1 = 0; a1 < test_counts[0]; a1++) {
            int arg1 = arg_test_vals[0][a1];
            for (a2 = 0; a2 < test_counts[1]; a2++) {
                int arg2 = arg_test_vals[1][a2];
                for (a3 = 0; a3 < test_counts[2]; a3++) {
                    int arg3 = arg_test_vals[2][a3];
                    // Ask btest_driver to run function
                    message_tag = ARGS_TAG;
                    if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                            write(STDOUT_FILENO, &arg1, sizeof(int)) == -1 ||
                            write(STDOUT_FILENO, &arg2, sizeof(int)) == -1 ||
                            write(STDOUT_FILENO, &arg3, sizeof(int)) == -1) {
                        perror("write");
                        exit(1);
                    }

                    // Read student result from btest_driver
                    if (read(STDIN_FILENO, &message_tag, sizeof(int)) == -1) {
                        perror("read");
                        exit(1);
                    }

                    if (message_tag == TIMEOUT_TAG || message_tag == SEGFAULT_TAG) {
                        // Give up, function has timed out and/or failed
                        return 1;
                    } else if (message_tag != FUNC_RETURN_TAG) {
                        fprintf(stderr, "Did not receive valid FUNC_RETURN_TAG from driver\n");
                        exit(1);
                    }

                    int student_result;
                    if (read(STDIN_FILENO, &student_result, sizeof(int)) == -1) {
                        perror("read");
                        exit(1);
                    }

                    errors += test_3_arg(student_result,
                             t->test_funct,
                             arg1,
                             arg2,
                             arg3,
                             t->name, &expected_result);
                    /* Stop testing if there is an error */
                    if (errors) {
                        message_tag = RESULT_FAILURE_TAG;
                        if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1 ||
                                write(STDOUT_FILENO, &expected_result, sizeof(int)) == -1) {
                            perror("write");
                            exit(1);
                        }
                        return 0;
                    }
                }
            }
        }
    }

    // If we reach this point, then no failures occurred along way
    message_tag = RESULT_SUCCESS_TAG;
    if (write(STDOUT_FILENO, &message_tag, sizeof(int)) == -1) {
        perror("write");
        exit(1);
    }
    return 0;
}

/*
 * run_tests - Run series of tests.  Return number of errors
 */
static void run_tests() {
    for (int i = 0; test_set[i].test_funct; i++) {
        if (!test_fname || strcmp(test_set[i].name,test_fname) == 0) {
            int timed_out = test_function(&test_set[i]);

            if (!timed_out) {
                // Wait for driver acknowledgement of test result
                int message_tag;
                if (read(STDIN_FILENO, &message_tag, sizeof(int)) == -1) {
                    perror("read");
                    exit(1);
                } else if (message_tag != RESULT_ACK_TAG) {
                    fprintf(stderr, "Did not receive valid RESULT_ACK_TAG from driver\n");
                    exit(1);
                }
            }
        }
    }
}

/*
 * get_num_val - Extract hex/decimal/or float value from string
 */
static int get_num_val(char *sval, unsigned *valp) {
    char *endp;

    /* See if it's an integer or floating point */
    int ishex = 0;
    int isfloat = 0;
    int i;
    for (i = 0; sval[i]; i++) {
        switch (sval[i]) {
            case 'x':
            case 'X':
                ishex = 1;
                break;
            case 'e':
            case 'E':
                if (!ishex)
                isfloat = 1;
                break;
            case '.':
                isfloat = 1;
                break;
            default:
                break;
        }
    }
    if (isfloat) {
        float fval = strtof(sval, &endp);
        if (!*endp) {
            *valp = *(unsigned *) &fval;
            return 1;
        }
        return 0;
    } else {
        long long int llval = strtoll(sval, &endp, 0);
        long long int upperbits = llval >> 31;
        /* will give -1 for negative, 0 or 1 for positive */
        if (!*valp && (upperbits == 0 || upperbits == -1 || upperbits == 1)) {
            *valp = (unsigned) llval;
            return 1;
        }
        return 0;
    }
}


/**************
 * Main routine
 **************/

int main(int argc, char *argv[]) {
    char c;

    /* parse command line args */
    while ((c = getopt(argc, argv, "hgf:r:T:1:2:3:")) != -1)
        switch (c) {
        case 'h': /* help */
        // Ignore this a it's handled by btest_driver
        break;
    case 'g': /* grading option for autograder */
        // Ignore this as it's handled in btest_driver
        break;
    case 'f': /* test only one function */
        test_fname = strdup(optarg);
        break;
    case 'r': /* set global rating for each problem */
        global_rating = atoi(optarg);
        if (global_rating < 0) {
            exit(1);
        }
        break;
    case '1': /* Get first argument */
        has_arg[0] = get_num_val(optarg, &argval[0]);
        if (!has_arg[0]) {
            exit(1);
        }
        break;
    case '2': /* Get first argument */
        has_arg[1] = get_num_val(optarg, &argval[1]);
        if (!has_arg[1]) {
            exit(1);
        }
        break;
    case '3': /* Get first argument */
        has_arg[2] = get_num_val(optarg, &argval[2]);
        if (!has_arg[2]) {
            exit(0);
        }
        break;
    case 'T': /* Set timeout limit */
        // Ignore this as it's handled by btest_driver
        break;
    default:
        // Should never happen as problems caught in btest_server
        exit(1);
    }

    /* test each function */
    run_tests();
    // Inform btest_driver that we're done
    int tag = DONE_TAG;
    if (write(STDOUT_FILENO, &tag, sizeof(int)) == -1) {
        perror("write");
        return 1;
    }

    return 0;
}
