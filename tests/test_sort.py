"""유닛 테스트 — 표준 라이브러리의 unittest만 쓴다.

실행: make test-py
"""

import random
import sys
import unittest
from pathlib import Path

# src/를 import 경로에 넣는다. 패키지로 만들지 않아도 되도록.
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "src"))

from sort import SORT_ALGORITHMS, selection_sort, shell_sort, tree_sort  # noqa: E402

DEMO = [6, 8, 5, 9, 10, 1, 7, 2, 4, 3]


def is_stable_result(records):
    return all(
        not (x[0] == y[0] and x[1] > y[1]) for x, y in zip(records, records[1:])
    )


class TestSorts(unittest.TestCase):
    """세 정렬에 같은 검사를 돌린다."""

    CASES = [
        [6, 8, 5, 9, 10, 1, 7, 2, 4, 3],
        [1, 2, 3, 4, 5],
        [5, 4, 3, 2, 1],
        [3, 1, 3, 1, 2],
        [7, 7, 7, 7],
        [0, -3, 5, -1, 2],
        [42],
        [],
    ]

    def test_basic_cases(self):
        for label, sort, _ in SORT_ALGORITHMS:
            for case in self.CASES:
                with self.subTest(algo=label, case=case):
                    a = list(case)
                    sort(a)
                    self.assertEqual(a, sorted(case))

    def test_random_against_sorted(self):
        rng = random.Random(2026)
        for label, sort, _ in SORT_ALGORITHMS:
            for n in range(0, 120):
                a = [rng.randrange(50) for _ in range(n)]
                want = sorted(a)
                sort(a)
                with self.subTest(algo=label, n=n):
                    self.assertEqual(a, want)

    def test_tree_sort_handles_long_chain(self):
        # 정렬된 입력은 트리를 높이 n의 한 줄로 만든다. 재귀였다면 한도(1000)에 걸린다.
        a = list(range(3000))
        tree_sort(a)
        self.assertEqual(a, list(range(3000)))


class TestCounts(unittest.TestCase):
    """tests/test_sort.c와 같은 숫자다. C와 Python이 같은 방식으로 세는지 본다."""

    def test_demo_counts(self):
        self.assertEqual(selection_sort(list(DEMO)), (45, 15))
        self.assertEqual(shell_sort(list(DEMO)), (29, 57))
        self.assertEqual(tree_sort(list(DEMO)), (23, 20))

    def test_selection_compares_fixed(self):
        n = 64
        self.assertEqual(selection_sort(list(range(n)))[0], n * (n - 1) // 2)
        self.assertEqual(selection_sort(list(range(n, 0, -1)))[0], n * (n - 1) // 2)

    def test_tree_sort_worst_case(self):
        n = 64
        comparisons, moves = tree_sort(list(range(n)))
        self.assertEqual(comparisons, n * (n - 1) // 2)
        self.assertEqual(moves, 2 * n)


class TestStability(unittest.TestCase):
    """(key, tag)를 key로만 정렬해 같은 key의 tag 순서가 지켜지는지 본다."""

    def sort_records(self, sort, keys):
        records = [(k, tag) for tag, k in enumerate(keys)]
        sort(records, key=lambda r: r[0])
        return records

    def test_selection_sort_is_unstable(self):
        # 최솟값 1을 맨 앞과 맞바꾸면서 (2,0)이 (2,1) 뒤로 넘어간다.
        got = self.sort_records(selection_sort, [2, 2, 1])
        self.assertEqual(got, [(1, 2), (2, 1), (2, 0)])

    def test_shell_sort_is_unstable(self):
        # gap 2에서 (0,3)이 (1,1) 자리로 뛰어넘어 (0,2)보다 앞에 선다.
        got = self.sort_records(shell_sort, [0, 1, 0, 0])
        self.assertEqual(got, [(0, 0), (0, 3), (0, 2), (1, 1)])

    def test_stability_matches_claim(self):
        rng = random.Random(7)
        keys = [rng.randrange(10) for _ in range(500)]
        for label, sort, stable in SORT_ALGORITHMS:
            with self.subTest(algo=label):
                self.assertEqual(is_stable_result(self.sort_records(sort, keys)), stable)


if __name__ == "__main__":
    unittest.main()
