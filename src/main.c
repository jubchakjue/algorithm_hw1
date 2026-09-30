/* 실행: make run-c     예제 배열을 세 정렬로 정렬해 본다
 *       make bench     세 정렬을 같은 조건으로 재서 표로 찍는다
 *       ./src/main.out --csv   같은 측정을 CSV로 (tools/plot.py가 읽는다)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bench.h"
#include "sort.h"

/* 무엇을 잴지는 이 표 한 곳에만 적는다. */
typedef struct Spec {
    const char *scope;  /* shapes: 입력 모양별, growth: n을 키우며 */
    InputKind kind;
    int n;
} Spec;

static const Spec SPECS[] = {
    {"shapes", INPUT_RANDOM, 4000},
    {"shapes", INPUT_SORTED, 4000},
    {"shapes", INPUT_REVERSED, 4000},
    {"shapes", INPUT_NEARLY, 4000},
    {"shapes", INPUT_FEW_UNIQUE, 4000},
    {"growth", INPUT_RANDOM, 1000},
    {"growth", INPUT_RANDOM, 2000},
    {"growth", INPUT_RANDOM, 4000},
    {"growth", INPUT_RANDOM, 8000},
    {"growth", INPUT_RANDOM, 16000},
    {"growth", INPUT_SORTED, 1000},
    {"growth", INPUT_SORTED, 2000},
    {"growth", INPUT_SORTED, 4000},
    {"growth", INPUT_SORTED, 8000},
    {"growth", INPUT_SORTED, 16000},
};
#define SPEC_COUNT ((int)(sizeof(SPECS) / sizeof(SPECS[0])))

enum { MIN_REPS = 3, SEED = 2026 };

/* 한 줄을 어디에 찍을지 (사람이 읽는 표 / CSV)를 갈아 끼운다. */
typedef void (*RowSink)(const Spec *spec, const SortAlgorithm *algo, const BenchResult *r);

static void printTableRow(const Spec *spec, const SortAlgorithm *algo, const BenchResult *r) {
    printf("%-7s %-13s %6d  %-14s %10.4f %6d %13lld %13lld %10lld %7d  %s\n",
           spec->scope, inputKindName(spec->kind), spec->n, algo->name,
           r->millis, r->reps, r->stats.comparisons, r->stats.moves,
           r->stats.extraBytes, r->stats.height, r->sorted ? "ok" : "FAIL");
}

static void printCsvRow(const Spec *spec, const SortAlgorithm *algo, const BenchResult *r) {
    printf("%s,%s,%d,%s,%.4f,%d,%lld,%lld,%lld,%d,%d\n",
           spec->scope, inputKindName(spec->kind), spec->n, algo->name,
           r->millis, r->reps, r->stats.comparisons, r->stats.moves,
           r->stats.extraBytes, r->stats.height, r->sorted);
}

static int runBench(RowSink sink) {
    int failures = 0;
    for (int s = 0; s < SPEC_COUNT; s++) {
        const Spec *spec = &SPECS[s];
        int *input = malloc((size_t)spec->n * sizeof *input);
        if (input == NULL) {
            fprintf(stderr, "out of memory (n = %d)\n", spec->n);
            return 1;
        }
        makeInput(input, spec->n, spec->kind, SEED);
        for (int k = 0; k < SORT_ALGORITHM_COUNT; k++) {
            BenchResult r = benchRun(&SORT_ALGORITHMS[k], input, spec->n, MIN_REPS);
            sink(spec, &SORT_ALGORITHMS[k], &r);
            failures += !r.sorted;
        }
        free(input);
    }
    return failures == 0 ? 0 : 1;
}

static void printArray(const int a[], int n) {
    for (int i = 0; i < n; i++) {
        printf(" %d", a[i]);
    }
}

/* Python의 main.py와 같은 줄을 찍는다. 두 구현이 같은 결과를 내는지 눈으로 본다. */
static void runDemo(void) {
    const int input[] = {6, 8, 5, 9, 10, 1, 7, 2, 4, 3};
    const int n = (int)(sizeof(input) / sizeof(input[0]));

    printf("input:");
    printArray(input, n);
    printf("\n");
    for (int k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        int a[sizeof(input) / sizeof(input[0])];
        SortStats stats;
        memcpy(a, input, sizeof(input));
        SORT_ALGORITHMS[k].sort(a, n, &stats);
        printf("%s:", SORT_ALGORITHMS[k].label);
        printArray(a, n);
        printf("  (comparisons = %lld, moves = %lld)\n", stats.comparisons, stats.moves);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--csv") == 0) {
        printf("scope,input,n,algo,millis,reps,comparisons,moves,extraBytes,height,sorted\n");
        return runBench(printCsvRow);
    }
    if (argc > 1 && strcmp(argv[1], "--bench") == 0) {
        printf("%-7s %-13s %6s  %-14s %10s %6s %13s %13s %10s %7s  %s\n",
               "scope", "input", "n", "algo", "ms", "reps", "comparisons", "moves",
               "extraBytes", "height", "check");
        return runBench(printTableRow);
    }
    runDemo();
    return 0;
}
