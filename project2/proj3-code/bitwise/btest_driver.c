#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "bits_impl.h"
#include "message_tags.h"

// Globals for signal handling
jmp_buf envbuf;
int timeout_limit = 10;

void handle_sig(int signo) {
    if (signo == SIGALRM) {
        // Timeout case
        siglongjmp(envbuf, 1);
    } else {
        // Segfault case
        siglongjmp(envbuf, 2);
    }
}

int install_sig_handler() {
    struct sigaction sigact;
    sigact.sa_handler = handle_sig;
    if (sigemptyset(&sigact.sa_mask)) {
        perror("sigemptyset");
        return -1;
    }
    sigact.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &sigact, NULL) == -1 || sigaction(SIGSEGV, &sigact, NULL) == -1) {
        perror("sigaction");
        return -1;
    }
    return 0;
}

/*
 * Usage - Display usage info
 */
static void usage(char *cmd) {
    printf("Usage: %s [-hg] [-r <n>] [-f <name> [-1|-2|-3 <val>]*] [-T <time limit>]\n", cmd);
    printf("  -1 <val>  Specify first function argument\n");
    printf("  -2 <val>  Specify second function argument\n");
    printf("  -3 <val>  Specify third function argument\n");
    printf("  -f <name> Test only the named function\n");
    printf("  -g        Compact output for grading (with no error msgs)\n");
    printf("  -h        Print this message\n");
    printf("  -r <n>    Give uniform weight of n for all problems\n");
    printf("  -T <lim>  Set timeout limit to lim\n");
    exit(1);
}

/*
 * get_num_val - Extract hex/decimal/or float value from string
 * *valp MUST be initialized to 0
 * Returns 1 if value successfuly parsed, 0 on failure
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

int main(int argc, char **argv) {
    int grade = 0; // Brief output just for grading purposes?
    timeout_limit = 10; // Timeout period for testing

    /* parse command line args */
    char c;
    while ((c = getopt(argc, argv, "hgf:r:T:1:2:3:")) != -1)
        switch (c) {
            // Ignore this, it's passed to btest
            case 'f':
            case 'r':
                break;

            // Just need to check that we can parse this, otherwise just pass it to btest
            // Apparently "optarg" is already defined for us with the getopt stuff
            case '1':
            case '2':
            case '3': {
                unsigned u = 0;
                if (get_num_val(optarg, &u) == 0) {
                    printf("Bad argument '%s'\n", optarg);
                    return 0;
                }
                break;
            }

            case 'g': /* grading option for autograder */
                grade = 1;
                break;

            case 'T': /* Set timeout limit */
                timeout_limit = atoi(optarg);
                break;

            case 'h': /* help */
                usage(argv[0]);
                return 0;

            default:
                usage(argv[0]);
                return 0;
    }

    int svs[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, svs) == -1) {
        perror("pipe");
        return 1;
    }

    // Set up a handler for timeout alarms
    if (install_sig_handler() != 0) {
        close(svs[0]);
        close(svs[1]);
        return 1;
    }

    // Set things up for btest_server external process
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        return 1;
    } else if (child_pid == 0) {
        // btest_server will read from stdin and stdout like normal
        if (dup2(svs[1], STDIN_FILENO) == -1) {
            perror("dup2");
            close(svs[0]);
            close(svs[1]);
            return 1;
        }
        if (dup2(svs[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            close(svs[0]);
            close(svs[1]);
            return 1;
        }

        // Copy command-line args for btest_server to use
        char *child_args[argc+1];
        child_args[0] = "btest_server";
        for (int i = 1; i < argc; i++) {
            child_args[i] = argv[i];
        }
        child_args[argc] = NULL; // Don't forget NULL sentinel at end

        // exec btest_server, 32-bit binary and all
        if (execv("./btest_server", child_args) == -1) {
            perror("exec");
            return 1;
        }
    } else {
        if (close(svs[1])) {
            close(svs[0]);
            return 1;
        }
    }

    // Print header
    printf("Score\tRating\tErrors\tFunction\n");

    // Parent now reads from pipe until btest_server is done
    int total_points_possible = 0;
    int total_points_earned = 0;
    int message_tag;
    int nbytes;
    int exit_code;

    // Remember information about most recently run function across loop iterations
    int function_id;
    int function_rating;
    int arg1, arg2, arg3;
    int num_args;
    int return_val;

    while ((nbytes = read(svs[0], &message_tag, sizeof(int))) > 0) {
        if (nbytes == -1) {
            perror("read");
            close(svs[0]);
            return 1;
        }

        switch(message_tag) {
            case NEW_FUNC_TAG: {
                // We're starting a batch of tests for a new function
                if (read(svs[0], &function_id, sizeof(int)) == -1 ||
                        read(svs[0], &function_rating, sizeof(int)) == -1) {
                    perror("read");
                    close(svs[0]);
                    return 1;
                }
                total_points_possible += function_rating;

                switch (function_id) {
                    case FITS_SHORT_TAG:
                    case ALL_EVEN_BITS_TAG:
                    case ANY_ODD_BIT_TAG:
                    case SIGN_TAG:
                    case IS_ASCII_DIGIT_TAG:
                    case ABS_VAL_TAG:
                    case FLOAT_SCALE_2_TAG: {
                        num_args = 1;
                        break;
                    }

                    case BIT_AND_TAG:
                    case IS_EQUAL_TAG:
                    case FLOAT_IS_EQUAL_TAG:
                    case FLOAT_IS_LESS_TAG:
                    case ROTATE_LEFT_TAG: {
                        num_args = 2;
                        break;
                    }

                    default: {
                        printf("Unknown function ID received from btest_server\n");
                        close(svs[0]);
                        return 1;
                    }
                }

                // Send acknowledgement back to server
                message_tag = NEW_FUNC_ACK_TAG;
                if (write(svs[0], &message_tag, sizeof(int)) == -1) {
                    perror("write");
                    close(svs[0]);
                    return 1;
                }
                break;
            }

            case ARGS_TAG: {
                // Server wants us to invoke function with specified arguments
                switch (num_args) {
                    case 0: {
                        // Do nothing
                        break;
                    }

                    case 1: {
                        if (read(svs[0], &arg1, sizeof(int)) == -1) {
                            perror("read");
                            close(svs[0]);
                            return 1;
                        }
                        break;
                    }

                    case 2: {
                        if (read(svs[0], &arg1, sizeof(int)) == -1 ||
                                read(svs[0], &arg2, sizeof(int)) == -1) {
                            perror("read");
                            close(svs[0]);
                            return 1;
                        }
                        break;
                    }

                    case 3: {
                        if (read(svs[0], &arg1, sizeof(int)) == -1 ||
                                read(svs[0], &arg2, sizeof(int)) == -1 ||
                                read(svs[0], &arg3, sizeof(int)) == -1) {
                            perror("read");
                            close(svs[0]);
                            return 1;
                        }
                        break;
                    }

                    default: {
                        printf("Error: Invalid number of function arguments\n");
                        close(svs[0]);
                        return 1;
                    }
                }

                // Jump back to here if timeout or segfault occurs
                int rc = sigsetjmp(envbuf, 1);
                if (rc == 1) {
                    // We jumped here from a timeout
                    // Don't write a return value, write a timeout alert to driver
                    message_tag = TIMEOUT_TAG;
                    if (write(svs[0], &message_tag, sizeof(int)) == -1) {
                        perror("write");
                        close(svs[0]);
                        return 1;
                    }
                    printf("ERROR: Test %s failed.\n  Timed out after %d secs (probably infinite loop)\n",
                           func_names[function_id], timeout_limit);
                    continue; // Get set to receive next message from btest_server
                } else if (rc == 2) {
                    // We jumped here because a student function segfaulted
                    // Don't write a return value, write a segfault alert to driver
                    message_tag = SEGFAULT_TAG;
                    if (write(svs[0], &message_tag, sizeof(int)) == -1) {
                        perror("write");
                        close(svs[0]);
                        return 1;
                    }
                    printf("ERROR: Test %s failed.\n  Segmentation Fault\n", func_names[function_id]);
                    continue; // Get set to receive next message from btest_driver
                }

                // Set up alarm signal for timeout
                if (timeout_limit > 0) {
                    alarm(timeout_limit);
                }

                switch(function_id) {
                    case BIT_AND_TAG: {
                        return_val = bitAnd(arg1, arg2);
                        break;
                    }

                    case FITS_SHORT_TAG: {
                        return_val = fitsShort(arg1);
                        break;
                    }

                    case ALL_EVEN_BITS_TAG: {
                        return_val = allEvenBits(arg1);
                        break;
                    }

                    case ANY_ODD_BIT_TAG: {
                        return_val = anyOddBit(arg1);
                        break;
                    }

                    case IS_EQUAL_TAG: {
                        return_val = isEqual(arg1, arg2);
                        break;
                    }

                    case FLOAT_IS_EQUAL_TAG: {
                        return_val = floatIsEqual(arg1, arg2);
                        break;
                    }

                    case SIGN_TAG: {
                        return_val = sign(arg1);
                        break;
                    }

                    case IS_ASCII_DIGIT_TAG: {
                        return_val = isAsciiDigit(arg1);
                        break;
                    }

                    case FLOAT_IS_LESS_TAG: {
                        return_val = floatIsLess(arg1, arg2);
                        break;
                    }

                    case ROTATE_LEFT_TAG: {
                        return_val = rotateLeft(arg1, arg2);
                        break;
                    }

                    case ABS_VAL_TAG: {
                        return_val = absVal(arg1);
                        break;
                    }

                    case FLOAT_SCALE_2_TAG: {
                        return_val = floatScale2(arg1);
                        break;
                    }

                    default: {
                        // Should never occur
                        printf("Error: Invalid function ID\n");
                        close(svs[0]);
                        return 1;
                    }
                }

                // Write return value back to server
                message_tag = FUNC_RETURN_TAG;
                if (write(svs[0], &message_tag, sizeof(int)) == -1 ||
                        write(svs[0], &return_val, sizeof(int)) == -1) {
                    perror("write");
                    close(svs[0]);
                    return 1;
                }
                break;
            }

            case RESULT_SUCCESS_TAG: {
                // Most recently invoked function has passed all tests
                printf(" %d\t%d\t%d\t%s\n", function_rating, function_rating, 0, func_names[function_id]);
                total_points_earned += function_rating;

                // Acknowledge receipt with server
                message_tag = RESULT_ACK_TAG;
                if (write(svs[0], &message_tag, sizeof(int)) == -1) {
                    perror("write");
                    close(svs[0]);
                    return 1;
                }
                break;
             }

            case RESULT_FAILURE_TAG: {
                int expected_result;
                if (read(svs[0], &expected_result, sizeof(int)) == -1) {
                    perror("read");
                    close(svs[0]);
                    return 1;
                }

                if (!grade) {
                    switch(num_args) {
                        case 0:
                            printf("ERROR: Test %s() failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                                func_names[function_id], return_val, return_val, expected_result, expected_result);
                            break;

                        case 1:
                            printf("ERROR: Test %s(%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                                func_names[function_id], arg1, arg1, return_val, return_val, expected_result, expected_result);
                            break;

                        case 2:
                            printf("ERROR: Test %s(%d[0x%x],%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                                func_names[function_id], arg1, arg1, arg2, arg2, return_val, return_val, expected_result, expected_result);
                            break;

                        case 3:
                            printf("ERROR: Test %s(%d[0x%x],%d[0x%x],%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n",
                                func_names[function_id], arg1, arg1, arg2, arg2, arg3, arg3, return_val, return_val, expected_result, expected_result);
                            break;

                        default:
                            // Should never happen
                            printf("Error: Invalid Number of Function Arguments Found\n");
                    }
                }
                printf(" %d\t%d\t%d\t%s\n", 0, function_rating, 1, func_names[function_id]);

                // Acknowledge receipt with server
                message_tag = RESULT_ACK_TAG;
                if (write(svs[0], &message_tag, sizeof(int)) == -1) {
                    perror("write");
                    close(svs[0]);
                    return 1;
                }
                break;
            }

            case DONE_TAG: {
                // Yep, a goto. We need to break from switch *and* loop
                goto END;
            }

            default: {
                // Should never occur
                printf("Error: Invalid message tag received from btest_server\n");
                close(svs[0]);
                return 1;
            }
        }
    }

END:
    printf("Total points: %d/%d\n", total_points_earned, total_points_possible);
    if (close(svs[0]) == -1) {
        perror("close");
        exit_code = 1;
    }
    // Wait for btest_server to exit
    if (wait(NULL) == -1) {
        perror("wait");
        exit_code = 1;
    }
    return exit_code;
}
