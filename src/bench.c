#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bench.h"

/* rand()는 구현마다 수열이 달라 기계가 바뀌면 입력도 바뀐다.
 * 어디서 돌려도 같은 입력이 나오도록 xorshift32를 직접 쓴다. */
static unsigned nextRandom(unsigned *state) {
    unsigned x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

const char *inputKindName(InputKind kind) {
    static const char *const NAMES[INPUT_KIND_COUNT] = {
        "random", "sorted", "reversed", "nearlySorted", "fewUnique",
    };
    return (kind >= 0 && kind < INPUT_KIND_COUNT) ? NAMES[kind] : "?";
}

void makeInput(int a[], int n, InputKind kind, unsigned seed) {
    unsigned state = seed ? seed : 1u;  /* xorshift는 0에서 멈춘다 */
    switch (kind) {
    case INPUT_RANDOM:
        for (int i = 0; i < n; i++) {
            a[i] = (int)(nextRandom(&state) % (unsigned)(10 * n));
        }
        break;
    case INPUT_SORTED:
        for (int i = 0; i < n; i++) {
            a[i] = i;
        }
        break;
    case INPUT_REVERSED:
        for (int i = 0; i < n; i++) {
            a[i] = n - 1 - i;
        }
        break;
    case INPUT_NEARLY:
        for (int i = 0; i < n; i++) {
            a[i] = i;
        }
        for (int s = 0; s < n / 100 + 1 && n > 1; s++) {
            int x = (int)(nextRandom(&state) % (unsigned)n);
            int y = (int)(nextRandom(&state) % (unsigned)n);
            int t = a[x];
            a[x] = a[y];
            a[y] = t;
        }
        break;
    case INPUT_FEW_UNIQUE:
        for (int i = 0; i < n; i++) {
            a[i] = (int)(nextRandom(&state) % 10u);
        }
        break;
    default:
        break;
    }
}

static int isAscending(const int a[], int n) {
    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) {
            return 0;
        }
    }
    return 1;
}

BenchResult benchRun(const SortAlgorithm *algo, const int input[], int n, int minReps) {
    BenchResult r = {0};
    int *work = malloc((size_t)(n > 0 ? n : 1) * sizeof *work);
    if (work == NULL) {
        fprintf(stderr, "benchRun: out of memory (n = %d)\n", n);
        return r;
    }

    double total = 0.0;
    r.sorted = 1;
    while (r.reps < minReps || (total < BENCH_MIN_MILLIS && r.reps < 100000)) {
        memcpy(work, input, (size_t)n * sizeof *work);
        clock_t start = clock();
        algo->sort(work, n, &r.stats);
        clock_t end = clock();
        total += (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        r.sorted = r.sorted && isAscending(work, n);
        r.reps++;
    }
    r.millis = total / r.reps;

    free(work);
    return r;
}
