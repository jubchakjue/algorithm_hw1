"""실행: make run-py

예제 배열을 세 정렬로 정렬해 본다. C의 main.c와 같은 줄을 찍는다.
그 뒤에 안정성을 실측한다 (C는 int 배열이라 같은 값을 구별할 수 없다).
"""

import random

from sort import SORT_ALGORITHMS


def is_stable_result(records):
    """(key, tag) 쌍이 key 순으로 놓였을 때, 같은 key끼리 tag가 오름차순인가."""
    return all(
        not (x[0] == y[0] and x[1] > y[1]) for x, y in zip(records, records[1:])
    )


def demo():
    data = [6, 8, 5, 9, 10, 1, 7, 2, 4, 3]
    print("input:", " ".join(str(x) for x in data))
    for label, sort, _ in SORT_ALGORITHMS:
        a = list(data)
        comparisons, moves = sort(a)
        print(
            f"{label}: {' '.join(str(x) for x in a)}"
            f"  (comparisons = {comparisons}, moves = {moves})"
        )


def stability():
    # key는 0~9 열 가지뿐이라 같은 key가 많다. tag에는 입력 순서를 새긴다.
    rng = random.Random(2026)
    records = [(rng.randrange(10), tag) for tag in range(1000)]
    print("stability (1000 records, 10 keys):")
    for label, sort, _ in SORT_ALGORITHMS:
        a = list(records)
        sort(a, key=lambda r: r[0])
        print(f"  {label}: {'안정' if is_stable_result(a) else '불안정'}")


if __name__ == "__main__":
    demo()
    stability()
