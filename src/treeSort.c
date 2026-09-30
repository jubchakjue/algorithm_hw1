/* 트리 정렬 — 이진 탐색 트리(BST)에 전부 넣고, 중위 순회로 꺼낸다.
 *
 * 노드는 malloc 한 번으로 배열째 잡고, 자식은 포인터 대신 인덱스로 가리킨다.
 * 삽입과 순회 모두 재귀를 쓰지 않는다. 정렬된 입력에서는 트리가 한 줄(높이 n)이
 * 되는데, 재귀로 짰다면 그때 호출 스택이 n단까지 쌓여 넘칠 수 있다.
 */
#include <stdlib.h>
#include "sort.h"

#define NIL (-1)

typedef struct TreeNode {
    int key;
    int left;   /* 왼쪽 자식의 인덱스. 없으면 NIL */
    int right;  /* 오른쪽 자식의 인덱스. 없으면 NIL */
} TreeNode;

void treeSort(int a[], int n, SortStats *stats) {
    *stats = (SortStats){0};
    if (n <= 0) {
        return;
    }

    TreeNode *nodes = malloc((size_t)n * sizeof *nodes);
    int *stack = malloc((size_t)n * sizeof *stack);  /* 중위 순회용. 깊이는 최대 n */
    if (nodes == NULL || stack == NULL) {
        free(nodes);
        free(stack);
        stats->extraBytes = -1;
        return;
    }
    stats->extraBytes = (long long)n * (long long)(sizeof *nodes + sizeof *stack);

    /* 1단계: a[0]을 뿌리로 두고 나머지를 차례로 삽입한다. */
    nodes[0] = (TreeNode){a[0], NIL, NIL};
    stats->moves++;
    stats->height = 1;
    for (int i = 1; i < n; i++) {
        int key = a[i];
        nodes[i] = (TreeNode){key, NIL, NIL};
        stats->moves++;

        int cur = 0;
        int depth = 1;  /* cur가 몇 층에 있나 */
        for (;;) {
            stats->comparisons++;
            /* '<'가 핵심이다. 같은 값은 오른쪽(= 나중)으로 가므로 순회에서 뒤에 나온다. */
            int *child = key < nodes[cur].key ? &nodes[cur].left : &nodes[cur].right;
            depth++;
            if (*child == NIL) {
                *child = i;
                break;
            }
            cur = *child;
        }
        if (depth > stats->height) {
            stats->height = depth;
        }
    }

    /* 2단계: 왼쪽 → 자기 → 오른쪽 순으로 방문하면 오름차순이다. */
    int top = 0;
    int k = 0;
    int cur = 0;
    while (cur != NIL || top > 0) {
        while (cur != NIL) {
            stack[top++] = cur;
            cur = nodes[cur].left;
        }
        cur = stack[--top];
        a[k++] = nodes[cur].key;
        stats->moves++;
        cur = nodes[cur].right;
    }

    free(nodes);
    free(stack);
}
