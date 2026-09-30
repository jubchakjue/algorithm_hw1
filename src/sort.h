/* 정렬 비교 과제 — 선택 정렬 · 셸 정렬 · 트리 정렬.
 *
 * 세 정렬은 모양이 같은 함수라 아래 SORT_ALGORITHMS 표 하나로 부를 수 있다.
 * 부르는 쪽(main.c · bench.c · 테스트)은 정렬 이름을 직접 적지 않는다.
 */
#ifndef SORT_H
#define SORT_H

/* 정렬이 한 번 도는 동안 센 값. 정렬 함수가 시작할 때 0으로 채운다. */
typedef struct SortStats {
    long long comparisons;  /* 원소끼리 비교한 횟수 */
    long long moves;        /* 원소를 옮겨 적은 횟수. 교환 한 번은 temp를 거치므로 3회 */
    long long extraBytes;   /* 입력 배열 밖에 malloc으로 잡은 바이트 (지역 변수는 세지 않는다) */
    int height;             /* 트리 정렬이 만든 트리의 높이(층 수). 나머지 정렬은 0 */
} SortStats;

/* a[0..n-1]을 제자리에서 오름차순으로 정렬한다. */
void selectionSort(int a[], int n, SortStats *stats);

/* gap을 n/2부터 절반씩 줄인다 (강의 예제와 같은 간격). */
void shellSort(int a[], int n, SortStats *stats);

/* 이진 탐색 트리에 모두 넣은 뒤 중위 순회로 꺼내 a에 다시 적는다.
 * 같은 값은 오른쪽으로 보내므로 안정 정렬이다. 트리는 균형을 잡지 않는다.
 * 메모리를 못 잡으면 a를 건드리지 않고 stats->extraBytes를 -1로 둔다. */
void treeSort(int a[], int n, SortStats *stats);

typedef struct SortAlgorithm {
    const char *name;       /* 함수 이름 (camelCase) */
    const char *label;      /* 출력용 이름 */
    int stable;             /* 안정 정렬인가 (이론). Python 테스트가 실측으로 확인한다 */
    void (*sort)(int a[], int n, SortStats *stats);
} SortAlgorithm;

extern const SortAlgorithm SORT_ALGORITHMS[];
extern const int SORT_ALGORITHM_COUNT;

#endif /* SORT_H */
