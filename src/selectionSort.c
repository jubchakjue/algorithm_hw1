/* 선택 정렬 — 남은 것 중 최솟값을 골라 앞으로 보낸다. */
#include "sort.h"

void selectionSort(int a[], int n, SortStats *stats) {
    *stats = (SortStats){0};
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        /* 입력과 상관없이 늘 끝까지 훑는다. 비교 횟수가 n(n-1)/2로 고정인 이유. */
        for (int j = i + 1; j < n; j++) {
            stats->comparisons++;
            if (a[j] < a[min]) {
                min = j;
            }
        }
        /* 멀리 떨어진 두 원소를 맞바꾸므로 같은 값의 순서가 뒤집힐 수 있다(불안정). */
        if (min != i) {
            int temp = a[i];
            a[i] = a[min];
            a[min] = temp;
            stats->moves += 3;
        }
    }
}
