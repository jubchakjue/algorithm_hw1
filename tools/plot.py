"""측정을 다시 돌려 report/ 아래에 CSV와 그래프(SVG)를 만든다.

실행: make charts
./src/main.out --csv의 출력을 그대로 report/results.csv로 남기고, 그것을 읽어 그린다.
사람이 읽는 표(--bench)를 파싱하지 않는다.
"""

import csv
import io
import math
import subprocess
from pathlib import Path

from svgchart import SERIES_COLORS, bar_chart, line_chart

ROOT = Path(__file__).resolve().parents[1]
REPORT = ROOT / "report"

ALGOS = ["selectionSort", "shellSort", "treeSort"]
LABELS = {"selectionSort": "선택 정렬", "shellSort": "셸 정렬", "treeSort": "트리 정렬"}
SHAPES = ["random", "sorted", "reversed", "nearlySorted", "fewUnique"]
SHAPE_LABELS = {
    "random": "무작위",
    "sorted": "정렬됨",
    "reversed": "역순",
    "nearlySorted": "거의 정렬",
    "fewUnique": "중복 많음",
}


def slope(xs, ys):
    """로그-로그 최소제곱 기울기 = 복잡도 지수의 추정값."""
    lx = [math.log(x) for x in xs]
    ly = [math.log(y) for y in ys]
    mx, my = sum(lx) / len(lx), sum(ly) / len(ly)
    num = sum((a - mx) * (b - my) for a, b in zip(lx, ly))
    den = sum((a - mx) ** 2 for a in lx)
    return num / den


def main():
    text = subprocess.run(
        [str(ROOT / "src" / "main.out"), "--csv"], check=True, capture_output=True, text=True
    ).stdout
    (REPORT / "results.csv").write_text(text, encoding="utf-8")
    rows = list(csv.DictReader(io.StringIO(text)))

    def pick(scope, inp, n, algo, field):
        for r in rows:
            if (r["scope"], r["input"], int(r["n"]), r["algo"]) == (scope, inp, n, algo):
                return float(r[field])
        raise KeyError((scope, inp, n, algo))

    groups = [SHAPE_LABELS[s] for s in SHAPES]
    series = [LABELS[a] for a in ALGOS]
    for field, name, ylabel in [
        ("comparisons", "shapes-comparisons", "비교 횟수 (로그)"),
        ("millis", "shapes-time", "시간 ms (로그)"),
    ]:
        values = [[pick("shapes", s, 4000, a, field) for s in SHAPES] for a in ALGOS]
        bar_chart(
            REPORT / f"{name}.svg",
            f"입력 모양별 {'비교 횟수' if field == 'comparisons' else '걸린 시간'} (n = 4,000)",
            groups, series, values, ylabel, log=True,
            subtitle="로그 축 — 눈금 한 칸이 10배",
        )

    heights = [[pick("shapes", s, 4000, "treeSort", "height") for s in SHAPES]]
    bar_chart(
        REPORT / "tree-height.svg",
        "트리 정렬이 만든 트리의 높이 (n = 4,000)",
        groups, ["트리 높이"], heights, "높이 (로그)", log=True,
        subtitle="균형이 맞으면 log2(4000) ≈ 12, 한 줄로 늘어서면 4,000",
        colors=[SERIES_COLORS[ALGOS.index("treeSort")]],
        ref=(math.log2(4000), "균형 트리의 높이 ≈ 12"),
    )

    ns = sorted({int(r["n"]) for r in rows if r["scope"] == "growth"})
    for inp, field, name, ylabel in [
        ("random", "comparisons", "growth-random-comparisons", "비교 횟수 (로그)"),
        ("sorted", "millis", "growth-sorted-time", "시간 ms (로그)"),
    ]:
        values = [[pick("growth", inp, n, a, field) for n in ns] for a in ALGOS]
        labels = [f"{LABELS[a]} (기울기 {slope(ns, v):.2f})" for a, v in zip(ALGOS, values)]
        what = "비교 횟수" if field == "comparisons" else "걸린 시간"
        line_chart(
            REPORT / f"{name}.svg",
            f"n을 키울 때 {what} — {SHAPE_LABELS[inp]} 입력",
            ns, labels, values, "n (로그)", ylabel, log=True,
            subtitle="로그-로그 축 — 기울기가 복잡도의 지수 (n² → 2, n log n → 1.1 안팎)",
        )
        print(f"{name}: " + ", ".join(labels))
    print(f"wrote {REPORT}/results.csv and 5 charts")


if __name__ == "__main__":
    main()
