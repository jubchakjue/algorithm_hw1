/* 측정 도구 — 입력을 만들고, 정렬 하나를 같은 조건으로 돌려 잰다.
 * 정렬 구현은 자기가 측정당하는 줄 모른다. */
#ifndef BENCH_H
#define BENCH_H

#include "sort.h"

typedef enum InputKind {
    INPUT_RANDOM,       /* 0 이상 10n 미만의 무작위 값 */
    INPUT_SORTED,       /* 0, 1, ..., n-1 */
    INPUT_REVERSED,     /* n-1, ..., 1, 0 */
    INPUT_NEARLY,       /* 정렬된 배열에서 n/100 쌍을 무작위로 맞바꾼 것 */
    INPUT_FEW_UNIQUE,   /* 0~9 열 가지 값뿐 (중복이 많다) */
    INPUT_KIND_COUNT
} InputKind;

const char *inputKindName(InputKind kind);

/* a[0..n-1]에 입력을 만든다. 씨앗이 같으면 언제나 같은 입력이 나온다. */
void makeInput(int a[], int n, InputKind kind, unsigned seed);

typedef struct BenchResult {
    double millis;      /* 한 번 정렬에 걸린 평균 시간. 입력 복사는 빼고 잰다 */
    int reps;           /* 실제로 돌린 횟수 */
    SortStats stats;    /* 마지막 회차의 값 (입력이 같으므로 매번 같다) */
    int sorted;         /* 결과가 오름차순인가 */
} BenchResult;

/* 적어도 minReps번, 그리고 합계가 BENCH_MIN_MILLIS를 넘을 때까지 되풀이해 평균낸다.
 * 0.01ms 수준의 짧은 정렬은 clock()의 눈금에 묻히므로 여러 번 돌려야 믿을 만하다. */
#define BENCH_MIN_MILLIS 20.0
BenchResult benchRun(const SortAlgorithm *algo, const int input[], int n, int minReps);

#endif /* BENCH_H */
