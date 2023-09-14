#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VEC_LEN 500000000

typedef struct timespec timespec_t;

double get_elapsed_time_sec(const timespec_t *start, const timespec_t *end) {
    long start_nanos = (long)1e9 * start->tv_sec + start->tv_nsec;
    long end_nanos = (long)1e9 * end->tv_sec + end->tv_nsec;
    return (double)(end_nanos - start_nanos) / 1e9;
}

// Computes and returns dot product of two integer sequences, 'a' and 'b'
int array_dot(const int *a, const int *b, int len) {
    int *temp = malloc(sizeof(int) * len);
    for (int i = 0; i < len; i++) {
        temp[i] = a[i] * b[i];
    }

    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += temp[i];
    }
    free(temp);
    return sum;
}

// Computes and returns dot product of two integer sequences, 'a' and 'b'
// Uses vector instructions to perform work in parallel
int array_dot_vec(const int *a, const int *b, int len) {
    // TODO Implement a vectorized dot product function
    int *temp = malloc(sizeof(int) * len);
    int i = 0;
    for (; i <= len - 8; i += 8) {
        __m256i a_vec = _mm256_loadu_si256((__m256i *) (a + i));
        __m256i b_vec = _mm256_loadu_si256((__m256i *) (b + i));

        __m256i temp_vec = _mm256_mullo_epi32(a_vec, b_vec);
        _mm256_storeu_si256((__m256i *) (temp + i), temp_vec);
    }

int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += temp[i];
    }
    free(temp);
    return sum;
}


void fill_array(int *v, int len) {
    for (int i = 0; i < len; i++) {
        v[i] = rand();
    }
}

int array_equals(const int *a, const int *b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

int main() {
    srand(2021);
    timespec_t start;
    timespec_t end;
    int *a = malloc(VEC_LEN * sizeof(int));
    fill_array(a, VEC_LEN);
    int *b = malloc(VEC_LEN * sizeof(int));
    fill_array(b, VEC_LEN);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    int x = array_dot(a, b, VEC_LEN);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    printf("array_dot: %.3f sec.\n", get_elapsed_time_sec(&start, &end));

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    int y = array_dot_vec(a, b, VEC_LEN);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    printf("array_dot_vec: %.3f sec.\n", get_elapsed_time_sec(&start, &end));
    if (x != y) {
        printf("Array dot functions produced inconsistent results\n");
    }

    free(a);
    free(b);
    return 0;
}
