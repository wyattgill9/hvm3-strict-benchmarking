#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef uint64_t Term;
typedef uint64_t Loc;
typedef uint8_t Lab;
typedef uint8_t Tag;

#define USE_HVM_LAYOUT 0

#if USE_HVM_LAYOUT
  // [Loc:54 | Lab:8 | Tag:4]
  #define TAG_BITS 4
  #define LAB_BITS 8
  #define LOC_BITS 54
#else
  // [Loc:48 | Lab:8 | Tag:8]
  #define TAG_BITS 8
  #define LAB_BITS 8
  #define LOC_BITS 48
#endif

#define TAG_MASK ((1ULL << TAG_BITS) - 1)   // 0xFF
#define LAB_MASK ((1ULL << LAB_BITS) - 1)   // 0xFF
#define LOC_MASK ((1ULL << LOC_BITS) - 1)   // 0xFFFFFFFFFFFF

Term term_new(Tag tag, Lab lab, Loc loc) {
    return ((Term)loc << (LAB_BITS + TAG_BITS)) |
           ((Term)lab << TAG_BITS) |
           ((Term)tag);
}

Term term_with_loc(Term term, Loc loc) {
    return (term & ~((Term)LOC_MASK << (LAB_BITS + TAG_BITS))) | 
           (((Term)loc & LOC_MASK) << (LAB_BITS + TAG_BITS));
}

Loc term_loc(Term term) {
    return term >> (LAB_BITS + TAG_BITS);
}

Lab term_lab(Term term) {
    return (term >> TAG_BITS) & LAB_MASK;
}

Tag term_tag(Term term) {
    return term & TAG_MASK;
}

double bench_diff_sec(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    const size_t ITER = 10000000000ULL;

    Term acc = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        acc ^= term_new(i % 256, i % 256, i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_new:           %.6f sec\n", bench_diff_sec(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        Term t = acc ^ i;
        acc ^= term_tag(t);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_tag:           %.6f sec\n", bench_diff_sec(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        Term t = acc ^ i;
        acc ^= term_lab(t);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_lab:           %.6f sec\n", bench_diff_sec(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        Term t = acc ^ i;
        acc ^= term_loc(t);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_loc:           %.6f sec\n", bench_diff_sec(start, end));

    printf("acc: %llu\n", (unsigned long long)acc);

    return 0;
}

