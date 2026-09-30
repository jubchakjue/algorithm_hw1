"""표준 모듈만으로 SVG 그래프를 그린다 (이미지에 matplotlib이 없다).

막대(묶음) 그래프와 꺾은선 그래프 두 가지만 있다. 로그 축을 고를 수 있다.
"""

import math
from xml.sax.saxutils import escape

# 계열 색은 고정 순서로 쓴다: 선택 · 셸 · 트리가 어느 그래프에서나 같은 색이다.
SERIES_COLORS = ["#2a78d6", "#eb6834", "#1baf7a"]
SURFACE = "#fcfcfb"
TEXT = "#0b0b0b"
TEXT_MUTED = "#52514e"
GRID = "#e4e3df"
AXIS = "#b9b8b3"
FONT = "font-family=\"'Noto Sans KR','Apple SD Gothic Neo','Malgun Gothic',sans-serif\""

WIDTH, HEIGHT = 720, 360
LEFT, RIGHT, TOP, BOTTOM = 78, 20, 64, 52


def _fmt(v):
    if v >= 1_000_000_000:
        return f"{v / 1_000_000_000:g}B"
    if v >= 1_000_000:
        return f"{v / 1_000_000:g}M"
    if v >= 1_000:
        return f"{v / 1_000:g}k"
    return f"{v:g}"


class _Scale:
    def __init__(self, lo, hi, log, pixel_lo, pixel_hi):
        self.log = log
        if log:
            lo = 10 ** math.floor(math.log10(lo))
            hi = 10 ** math.ceil(math.log10(hi))
        else:
            lo = 0
            step = self._nice_step(hi / 5)
            hi = math.ceil(hi / step) * step
        self.lo, self.hi = lo, hi
        self.p0, self.p1 = pixel_lo, pixel_hi

    @staticmethod
    def _nice_step(raw):
        mag = 10 ** math.floor(math.log10(raw))
        for m in (1, 2, 2.5, 5, 10):
            if raw <= m * mag:
                return m * mag
        return 10 * mag

    def __call__(self, v):
        if self.log:
            t = (math.log10(v) - math.log10(self.lo)) / (
                math.log10(self.hi) - math.log10(self.lo)
            )
        else:
            t = (v - self.lo) / (self.hi - self.lo)
        return self.p0 + t * (self.p1 - self.p0)

    def ticks(self):
        if self.log:
            e0, e1 = round(math.log10(self.lo)), round(math.log10(self.hi))
            return [10**e for e in range(e0, e1 + 1)]
        step = self._nice_step(self.hi / 5)
        return [i * step for i in range(round(self.hi / step) + 1)]


def _frame(title, subtitle):
    out = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{HEIGHT}" '
        f'viewBox="0 0 {WIDTH} {HEIGHT}" {FONT}>',
        f'<rect width="{WIDTH}" height="{HEIGHT}" fill="{SURFACE}"/>',
        f'<text x="{LEFT}" y="24" font-size="16" font-weight="600" fill="{TEXT}">'
        f"{escape(title)}</text>",
    ]
    if subtitle:
        out.append(
            f'<text x="{LEFT}" y="42" font-size="12" fill="{TEXT_MUTED}">'
            f"{escape(subtitle)}</text>"
        )
    return out


def _y_axis(out, ys, ylabel):
    for t in ys.ticks():
        y = ys(t)
        out.append(
            f'<line x1="{LEFT}" x2="{WIDTH - RIGHT}" y1="{y:.1f}" y2="{y:.1f}" '
            f'stroke="{GRID}" stroke-width="1"/>'
        )
        out.append(
            f'<text x="{LEFT - 8}" y="{y + 4:.1f}" font-size="11" text-anchor="end" '
            f'fill="{TEXT_MUTED}">{_fmt(t)}</text>'
        )
    mid = (TOP + HEIGHT - BOTTOM) / 2
    out.append(
        f'<text x="16" y="{mid:.1f}" font-size="12" fill="{TEXT_MUTED}" '
        f'text-anchor="middle" transform="rotate(-90 16 {mid:.1f})">{escape(ylabel)}</text>'
    )


def _text_width(text):
    """12px 글꼴에서 대략의 폭. 한글은 영문보다 두 배 가까이 넓다."""
    return sum(11.5 if ord(ch) > 0x2E80 else 6.8 for ch in text)


def _legend(out, names, colors):
    x = WIDTH - RIGHT
    # 오른쪽 끝에서 왼쪽으로 쌓는다.
    items = []
    for i, name in reversed(list(enumerate(names))):
        x -= 17 + _text_width(name) + 14
        items.append((x, i, name))
    for x, i, name in items:
        out.append(
            f'<rect x="{x:.1f}" y="{TOP - 16}" width="12" height="12" rx="3" '
            f'fill="{colors[i]}"/>'
        )
        out.append(
            f'<text x="{x + 17:.1f}" y="{TOP - 6}" font-size="12" fill="{TEXT}">'
            f"{escape(name)}</text>"
        )


def bar_chart(path, title, groups, series, values, ylabel, log=False, subtitle="",
              colors=None, ref=None):
    """values[s][g] — 계열 s, 묶음 g의 값. 로그 축이면 0 이하 값은 막대를 그리지 않는다.

    colors를 주면 계열 색을 바꾼다 (계열이 하나뿐일 때 그 알고리즘의 색을 쓰려고).
    ref=(값, 이름)을 주면 그 높이에 점선 기준선을 긋는다.
    """
    colors = colors or SERIES_COLORS
    flat = [v for row in values for v in row if v > 0]
    ys = _Scale(min(flat + [ref[0]] if ref else flat), max(flat), log, HEIGHT - BOTTOM, TOP)
    out = _frame(title, subtitle)
    _y_axis(out, ys, ylabel)
    if len(series) > 1:
        _legend(out, series, colors)

    plot_w = WIDTH - LEFT - RIGHT
    group_w = plot_w / len(groups)
    bar_w = min(28, (group_w - 24) / len(series))
    base = HEIGHT - BOTTOM
    for g, gname in enumerate(groups):
        cx = LEFT + group_w * (g + 0.5)
        x0 = cx - bar_w * len(series) / 2
        for s in range(len(series)):
            v = values[s][g]
            if v <= 0 or (log and v < ys.lo):
                continue
            top = ys(v)
            x = x0 + s * bar_w + 1  # 이웃 막대와 2px 틈
            w = bar_w - 2
            r = min(4, w / 2, base - top)
            # 윗모서리만 둥글게: 막대는 바닥에 붙어 있다.
            out.append(
                f'<path d="M{x:.1f},{base} V{top + r:.1f} Q{x:.1f},{top:.1f} {x + r:.1f},{top:.1f} '
                f'H{x + w - r:.1f} Q{x + w:.1f},{top:.1f} {x + w:.1f},{top + r:.1f} V{base} Z" '
                f'fill="{colors[s]}"><title>{escape(series[s])} · {escape(gname)}: '
                f"{v:,.3f}</title></path>".replace(".000<", "<")
            )
        out.append(
            f'<text x="{cx:.1f}" y="{base + 18}" font-size="12" text-anchor="middle" '
            f'fill="{TEXT}">{escape(gname)}</text>'
        )
    out.append(
        f'<line x1="{LEFT}" x2="{WIDTH - RIGHT}" y1="{base}" y2="{base}" stroke="{AXIS}"/>'
    )
    if ref:
        y = ys(ref[0])
        out.append(
            f'<line x1="{LEFT}" x2="{WIDTH - RIGHT}" y1="{y:.1f}" y2="{y:.1f}" '
            f'stroke="{TEXT_MUTED}" stroke-width="1.5" stroke-dasharray="5 4"/>'
        )
        # 이름은 막대와 겹치지 않도록 범례 자리(오른쪽 위)에 단다.
        lx = WIDTH - RIGHT - _text_width(ref[1]) - 30
        out.append(
            f'<line x1="{lx:.1f}" x2="{lx + 22:.1f}" y1="{TOP - 10}" y2="{TOP - 10}" '
            f'stroke="{TEXT_MUTED}" stroke-width="1.5" stroke-dasharray="5 4"/>'
        )
        out.append(
            f'<text x="{lx + 28:.1f}" y="{TOP - 6}" font-size="12" fill="{TEXT}">'
            f"{escape(ref[1])}</text>"
        )
    out.append("</svg>")
    _write(path, out)


def line_chart(path, title, xs, series, values, xlabel, ylabel, log=True, subtitle=""):
    """values[s][i] — 계열 s의 xs[i]에서의 값. log면 두 축 모두 로그."""
    flat = [v for row in values for v in row if v > 0]
    ys = _Scale(min(flat), max(flat), log, HEIGHT - BOTTOM, TOP)
    xs_scale = _Scale(min(xs), max(xs), log, LEFT + 24, WIDTH - RIGHT - 24)
    # x축 눈금은 실제로 잰 n에만 단다.
    xs_scale.lo, xs_scale.hi = (min(xs), max(xs)) if log else (0, max(xs))
    out = _frame(title, subtitle)
    _y_axis(out, ys, ylabel)
    _legend(out, series, SERIES_COLORS)

    base = HEIGHT - BOTTOM
    for x in xs:
        px = xs_scale(x)
        out.append(
            f'<text x="{px:.1f}" y="{base + 18}" font-size="11" text-anchor="middle" '
            f'fill="{TEXT_MUTED}">{x:,}</text>'
        )
    out.append(
        f'<text x="{(LEFT + WIDTH - RIGHT) / 2:.1f}" y="{HEIGHT - 10}" font-size="12" '
        f'text-anchor="middle" fill="{TEXT_MUTED}">{escape(xlabel)}</text>'
    )
    out.append(
        f'<line x1="{LEFT}" x2="{WIDTH - RIGHT}" y1="{base}" y2="{base}" stroke="{AXIS}"/>'
    )
    for s, row in enumerate(values):
        pts = [(xs_scale(x), ys(v)) for x, v in zip(xs, row) if v > 0]
        d = " ".join(f"{'M' if i == 0 else 'L'}{x:.1f},{y:.1f}" for i, (x, y) in enumerate(pts))
        out.append(
            f'<path d="{d}" fill="none" stroke="{SERIES_COLORS[s]}" stroke-width="2" '
            f'stroke-linejoin="round"/>'
        )
        for (x, y), v in zip(pts, row):
            out.append(
                f'<circle cx="{x:.1f}" cy="{y:.1f}" r="4" fill="{SERIES_COLORS[s]}" '
                f'stroke="{SURFACE}" stroke-width="2"><title>{escape(series[s])}: '
                f"{v:,.3f}</title></circle>".replace(".000<", "<")
            )
    out.append("</svg>")
    _write(path, out)


def _write(path, lines):
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")
