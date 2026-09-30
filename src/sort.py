"""정렬 비교 과제 — 선택 정렬 · 셸 정렬 · 트리 정렬.

C 구현(src/*.c)과 같은 알고리즘, 같은 방식으로 센다. 세 함수 모두 a를 제자리에서
오름차순으로 정렬하고 (비교 횟수, 이동 횟수)를 돌려준다. 교환 한 번은 이동 3회다.

key를 주면 원소 대신 key(원소)로 비교한다. 안정성을 재려고 둔 인자다.
"""


def _identity(x):
    return x


def selection_sort(a, key=None):
    """남은 것 중 최솟값을 골라 앞으로 보낸다."""
    key = key or _identity
    comparisons = 0
    moves = 0
    n = len(a)
    for i in range(n - 1):
        min_i = i
        for j in range(i + 1, n):
            comparisons += 1
            if key(a[j]) < key(a[min_i]):
                min_i = j
        if min_i != i:
            a[i], a[min_i] = a[min_i], a[i]
            moves += 3
    return comparisons, moves


def shell_sort(a, key=None):
    """gap을 n/2부터 절반씩 줄이며 gap 간격의 삽입 정렬을 한다."""
    key = key or _identity
    comparisons = 0
    moves = 0
    n = len(a)
    gap = n // 2
    while gap >= 1:
        for i in range(gap, n):
            item = a[i]
            moves += 1
            j = i - gap
            while j >= 0:
                comparisons += 1
                if key(a[j]) <= key(item):
                    break
                a[j + gap] = a[j]
                moves += 1
                j -= gap
            a[j + gap] = item
            moves += 1
        gap //= 2
    return comparisons, moves


NIL = -1


def tree_sort(a, key=None):
    """이진 탐색 트리에 모두 넣은 뒤 중위 순회로 꺼내 a에 다시 적는다.

    같은 값은 오른쪽으로 보내므로 안정 정렬이다. 트리는 균형을 잡지 않는다.
    정렬된 입력에서 트리 높이가 n이 되므로 재귀 대신 반복문으로 짰다
    (Python의 기본 재귀 한도는 1000이다).
    """
    key = key or _identity
    comparisons = 0
    moves = 0
    n = len(a)
    if n == 0:
        return comparisons, moves

    # 노드 i의 값과 자식을 세 리스트에 나눠 담는다 (C의 TreeNode 배열과 같은 모양).
    items = [None] * n
    left = [NIL] * n
    right = [NIL] * n

    items[0] = a[0]
    moves += 1
    for i in range(1, n):
        items[i] = a[i]
        moves += 1
        k = key(a[i])
        cur = 0
        while True:
            comparisons += 1
            if k < key(items[cur]):
                if left[cur] == NIL:
                    left[cur] = i
                    break
                cur = left[cur]
            else:
                if right[cur] == NIL:
                    right[cur] = i
                    break
                cur = right[cur]

    # 왼쪽 → 자기 → 오른쪽 순으로 방문하면 오름차순이다.
    stack = []
    out = 0
    cur = 0
    while cur != NIL or stack:
        while cur != NIL:
            stack.append(cur)
            cur = left[cur]
        cur = stack.pop()
        a[out] = items[cur]
        out += 1
        moves += 1
        cur = right[cur]
    return comparisons, moves


# C의 SORT_ALGORITHMS 표와 같은 순서. (출력용 이름, 함수, 안정 정렬인가)
SORT_ALGORITHMS = [
    ("선택 정렬", selection_sort, False),
    ("셸 정렬", shell_sort, False),
    ("트리 정렬", tree_sort, True),
]
