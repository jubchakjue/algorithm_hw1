#include "sort.h"

/* 정렬을 하나 더 넣으면 이 표에 한 줄 넣는다. 벤치마크와 테스트가 표를 훑는다. */
const SortAlgorithm SORT_ALGORITHMS[] = {
    {"selectionSort", "선택 정렬", 0, selectionSort},
    {"shellSort",     "셸 정렬",   0, shellSort},
    {"treeSort",      "트리 정렬", 1, treeSort},
};

const int SORT_ALGORITHM_COUNT =
    (int)(sizeof(SORT_ALGORITHMS) / sizeof(SORT_ALGORITHMS[0]));
