/* 유닛 테스트 — 외부 프레임워크 없이 표준 C만 쓴다.
 * 실행: make test-c
 *
 * 정렬마다 따로 쓰지 않고 SORT_ALGORITHMS 표를 훑는다. 정렬을 하나 더 넣으면
 * 그 순간부터 같은 검사를 받는다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bench.h"
#include "sort.h"

static int checks = 0;
static int failures = 0;

static void check(int ok, const char *algo, const char *name) {
    checks++;
    if (!ok) {
        failures++;
        printf("FAIL  %-14s %s\n", algo, name);
        return;
    }
    printf("ok    %-14s %s\n", algo, name);
}

static int compareInt(const void *x, const void *y) {
    int a = *(const int *)x;
    int b = *(const int *)y;
    return (a > b) - (a < b);
}

/* input을 정렬한 결과가 qsort의 결과와 같은지 본다. */
static int sortsLikeQsort(const SortAlgorithm *algo, const int input[], int n) {
    int *got = malloc((size_t)(n > 0 ? n : 1) * sizeof *got);
    int *want = malloc((size_t)(n > 0 ? n : 1) * sizeof *want);
    if (got == NULL || want == NULL) {
        free(got);
        free(want);
        return 0;
    }
    memcpy(got, input, (size_t)n * sizeof *got);
    memcpy(want, input, (size_t)n * sizeof *want);
    SortStats stats;
    algo->sort(got, n, &stats);
    qsort(want, (size_t)n, sizeof *want, compareInt);
    int same = n == 0 || memcmp(got, want, (size_t)n * sizeof *got) == 0;
    free(got);
    free(want);
    return same;
}

static void testBasicCases(const SortAlgorithm *algo) {
    static const struct {
        const char *name;
        int n;
        int a[10];
    } CASES[] = {
        {"섞인 배열", 10, {6, 8, 5, 9, 10, 1, 7, 2, 4, 3}},
        {"이미 정렬된 배열", 5, {1, 2, 3, 4, 5}},
        {"역순 배열", 5, {5, 4, 3, 2, 1}},
        {"중복이 있는 배열", 5, {3, 1, 3, 1, 2}},
        {"모두 같은 값", 4, {7, 7, 7, 7}},
        {"음수 섞임", 5, {0, -3, 5, -1, 2}},
        {"원소 하나", 1, {42}},
        {"빈 배열", 0, {0}},
    };
    for (size_t c = 0; c < sizeof(CASES) / sizeof(CASES[0]); c++) {
        check(sortsLikeQsort(algo, CASES[c].a, CASES[c].n), algo->name, CASES[c].name);
    }
}

/* 입력 모양 다섯 가지 × 크기 0..200을 전부 qsort와 맞춰 본다.
 * 셸 정렬의 gap 경계, 트리의 한쪽 쏠림 같은 것이 특정 크기에서만 깨지기 쉽다. */
static void testSweep(const SortAlgorithm *algo) {
    int input[200];
    int ok = 1;
    for (int kind = 0; kind < INPUT_KIND_COUNT && ok; kind++) {
        for (int n = 0; n <= 200 && ok; n++) {
            makeInput(input, n, (InputKind)kind, 7u + (unsigned)n);
            ok = sortsLikeQsort(algo, input, n);
        }
    }
    check(ok, algo->name, "입력 모양 5가지 x n = 0..200 을 qsort와 대조");
}

static SortStats statsOf(void (*sort)(int[], int, SortStats *), const int input[], int n) {
    int a[64];
    SortStats stats;
    memcpy(a, input, (size_t)n * sizeof *a);
    sort(a, n, &stats);
    return stats;
}

/* 센 값이 이론과 맞는지 본다. 여기 적은 숫자는 tests/test_sort.py에도 똑같이 있다.
 * C와 Python이 같은 알고리즘을 같은 방식으로 세는지 확인하는 셈이다. */
static void testCounts(void) {
    const int demo[] = {6, 8, 5, 9, 10, 1, 7, 2, 4, 3};
    SortStats s;

    s = statsOf(selectionSort, demo, 10);
    check(s.comparisons == 45 && s.moves == 15, "selectionSort", "예제 배열: 비교 45, 이동 15");
    s = statsOf(shellSort, demo, 10);
    check(s.comparisons == 29 && s.moves == 57, "shellSort", "예제 배열: 비교 29, 이동 57");
    s = statsOf(treeSort, demo, 10);
    check(s.comparisons == 23 && s.moves == 20, "treeSort", "예제 배열: 비교 23, 이동 20");

    int sorted[64];
    int random[64];
    makeInput(sorted, 64, INPUT_SORTED, 1u);
    makeInput(random, 64, INPUT_RANDOM, 1u);

    /* 선택 정렬은 입력과 상관없이 비교가 n(n-1)/2로 고정이다. */
    check(statsOf(selectionSort, sorted, 64).comparisons == 64 * 63 / 2 &&
              statsOf(selectionSort, random, 64).comparisons == 64 * 63 / 2,
          "selectionSort", "비교 횟수는 입력과 무관하게 n(n-1)/2");
    check(statsOf(selectionSort, sorted, 64).moves == 0, "selectionSort", "정렬된 입력은 이동 0");

    /* 트리 정렬: 정렬된 입력이면 트리가 한 줄이 되어 높이 n, 비교 n(n-1)/2. */
    s = statsOf(treeSort, sorted, 64);
    check(s.height == 64 && s.comparisons == 64 * 63 / 2, "treeSort",
          "정렬된 입력: 높이 n, 비교 n(n-1)/2 (최악)");
    s = statsOf(treeSort, random, 64);
    check(s.height < 20, "treeSort", "무작위 입력: 높이가 n보다 한참 낮다");
    check(s.moves == 2 * 64, "treeSort", "이동은 늘 2n (트리에 넣기 n + 꺼내기 n)");
    check(s.extraBytes == 64 * (long long)(3 * sizeof(int) + sizeof(int)), "treeSort",
          "추가 메모리는 노드 n개 + 스택 n칸");
    check(statsOf(selectionSort, random, 64).extraBytes == 0 &&
              statsOf(shellSort, random, 64).extraBytes == 0,
          "selection/shell", "추가 메모리 0 (제자리 정렬)");
}

static void testInputs(void) {
    int a[100];
    makeInput(a, 100, INPUT_FEW_UNIQUE, 3u);
    int ok = 1;
    for (int i = 0; i < 100; i++) {
        ok = ok && a[i] >= 0 && a[i] < 10;
    }
    check(ok, "makeInput", "fewUnique는 0~9만 만든다");

    int b[100];
    makeInput(a, 100, INPUT_RANDOM, 5u);
    makeInput(b, 100, INPUT_RANDOM, 5u);
    check(memcmp(a, b, sizeof a) == 0, "makeInput", "같은 씨앗이면 같은 입력");
}

int main(void) {
    for (int k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        testBasicCases(&SORT_ALGORITHMS[k]);
        testSweep(&SORT_ALGORITHMS[k]);
    }
    testCounts();
    testInputs();

    printf("\n%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
