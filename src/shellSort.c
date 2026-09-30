/* 셸 정렬 — 멀리 떨어진 원소끼리 먼저 삽입 정렬해 두고, 간격을 줄여 간다. */
#include "sort.h"

void shellSort(int a[], int n, SortStats *stats) {
    *stats = (SortStats){0};
    for (int gap = n / 2; gap >= 1; gap /= 2) {
        /* gap만큼 떨어진 원소들로 이뤄진 부분 배열마다 삽입 정렬을 한다. */
        for (int i = gap; i < n; i++) {
            int key = a[i];
            stats->moves++;
            int j = i - gap;
            while (j >= 0) {
                stats->comparisons++;
                if (a[j] <= key) {
                    break;
                }
                a[j + gap] = a[j];
                stats->moves++;
                j -= gap;
            }
            a[j + gap] = key;
            stats->moves++;
        }
    }
}
