#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef uint64_t Term;
typedef uint64_t Loc;
typedef uint8_t Lab;
typedef uint8_t Tag;
typedef __uint128_t Pair;

#define TAG_BITS 4
#define LAB_BITS 8 
#define LOC_BITS 52 
#define TAG_MASK ((1ULL << TAG_BITS) - 1)
#define LAB_MASK ((1ULL << LAB_BITS) - 1)
#define LOC_MASK ((1ULL << LOC_BITS) - 1)

Term term_new(Tag tag, Lab lab, Loc loc) {
    return (((Term)tag & TAG_MASK) << (LAB_BITS + LOC_BITS)) |
           (((Term)lab & LAB_MASK) << LOC_BITS) |
           ((Term)loc & LOC_MASK);
}

Term term_with_loc(Term term, Loc loc) {
    return (term & ~LOC_MASK) | ((Term)loc & LOC_MASK);
}

Loc term_loc(Term term) {
    return term & LOC_MASK;
}

Lab term_lab(Term term) {
    return (term >> LOC_BITS) & LAB_MASK;
}

Tag term_tag(Term term) {
    return (term >> (LOC_BITS + LAB_BITS)) & TAG_MASK;
}

double bench_diff_sec(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    const size_t ITER = 100000000;

    Term acc = 0;
    struct timespec start, end;

    // Benchmark term_new
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        acc ^= term_new(i % 16, i % 256, i);  // tag: 4-bit, lab: 8-bit, loc: 54-bit
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_new:      %.6f sec\n", bench_diff_sec(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        Term t = acc ^ i;
        acc ^= term_tag(t);
        acc ^= term_lab(t);
        acc ^= term_loc(t);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_{tag,lab,loc}: %.6f sec\n", bench_diff_sec(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITER; ++i) {
        acc ^= term_with_loc(acc, i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("term_with_loc: %.6f sec\n", bench_diff_sec(start, end));

    printf("acc: %llu\n", (unsigned long long)acc);
    return 0;
}
