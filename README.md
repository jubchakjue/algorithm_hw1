# 과제 1. 정렬 비교 — 선택 · 셸 · 트리 정렬

2026-2 **고급알고리즘**(SIT2001-01) 과제 1. 배운 정렬 두 개(**선택 정렬**, **셸 정렬**)와
배우지 않은 정렬 하나(**트리 정렬**)를 C와 Python으로 구현하고 같은 조건에서 비교했다.

- **보고서: [report/REPORT.md](report/REPORT.md)**
- 실습 환경: [lec-algorithm/algorithm-env](https://github.com/lec-algorithm/algorithm-env) template에서 시작

## 준비물

**GitHub 계정 하나면 됩니다.** 로컬에서 돌리려면 Git과 Docker가 필요합니다.
컴파일러와 Python은 컨테이너 이미지 안에 들어 있어 따로 설치하지 않습니다.

## 시작하기 (권장): Codespaces

1. 이 저장소 상단의 **Use this template** → **Create a new repository**
2. 저장소 이름을 정합니다 (예: `algorithms-hw1`, `my-algorithm-project`)
3. 만들어진 **내 저장소**에서 **Code** → **Codespaces** 탭
4. **Create codespace on main**

잠시 기다리면 브라우저에 VS Code가 뜹니다. **그 터미널이 곧 컨테이너 안**이므로
바로 아래 [돌려보기](#돌려보기)로 넘어가면 됩니다.

## 로컬에서 하기

위와 같이 **내 저장소를 먼저 만든 뒤** 그것을 클론합니다.

```sh
git clone https://github.com/<본인 계정>/<내 저장소>.git
cd <내 저장소>
docker compose up -d
docker compose exec lab bash
```

처음 한 번은 이미지를 받느라 몇 분 걸립니다. 이후에는 몇 초면 뜹니다.
**이후 모든 `docker compose` 명령은 이 폴더에서 칩니다.**

VS Code를 쓴다면 Dev Containers 확장의 **Reopen in Container**를 골라도
됩니다. Codespaces와 같은 설정을 씁니다.

## 돌려보기

컨테이너 안에서 `make` 한 단어면 됩니다.

- 실행

```sh
make run
```

- 결과 (C · Python 순서로 같은 줄이 두 번 나오고, 끝에 Python의 안정성 실측이 붙는다)

```console
input: 6 8 5 9 10 1 7 2 4 3
선택 정렬: 1 2 3 4 5 6 7 8 9 10  (comparisons = 45, moves = 15)
셸 정렬: 1 2 3 4 5 6 7 8 9 10  (comparisons = 29, moves = 57)
트리 정렬: 1 2 3 4 5 6 7 8 9 10  (comparisons = 23, moves = 20)
...
stability (1000 records, 10 keys):
  선택 정렬: 불안정
  셸 정렬: 불안정
  트리 정렬: 안정
```

C와 Python 두 구현이 같은 결과(정렬 결과와 비교 · 이동 횟수)를 냅니다.

## 테스트

- 실행

```sh
make test
```

- 결과

```console
ok    selectionSort  섞인 배열
...
ok    makeInput      같은 씨앗이면 같은 입력

39 checks, 0 failures
...
Ran 9 tests in 0.3s

OK
```

테스트가 하나라도 실패하면 `make`가 0이 아닌 코드로 끝납니다. 과제를 내기
전에 이 명령이 통과하는지 확인하세요.

| 명령 | 하는 일 |
| --- | --- |
| `make run` | 예제 실행 (C · Python) |
| `make test` | 유닛 테스트 (C · Python) |
| `make bench` | 세 정렬을 같은 조건으로 재서 표로 출력 (몇 초 걸린다) |
| `make charts` | 다시 재서 `report/results.csv`와 그래프(SVG)를 만든다 |
| `make run-c` · `make run-py` | 한쪽만 실행 |
| `make test-c` · `make test-py` | 한쪽만 테스트 |
| `make debug` | 디버그 심볼을 넣어 빌드 |
| `make clean` | 빌드 산출물 정리 |

## VS Code에서 실행·디버그

Codespaces나 Dev Containers로 열었다면 편집기에서 바로 됩니다.

| 하고 싶은 것 | 방법 |
| --- | --- |
| 파일 하나 실행 | 편집기 오른쪽 위 **▶ 버튼** (Code Runner) |
| 전체 실행 | `Cmd/Ctrl + Shift + B` (기본 빌드 작업이 `make run`) |
| 테스트 | 명령 팔레트 → **Tasks: Run Test Task** |
| C 디버그 | `F5` → **C 디버그 (현재 파일)** |
| Python 디버그 | `F5` → **Python 디버그 (현재 파일)** |

`F5`를 누르면 빌드가 먼저 돌아 심볼이 있는 바이너리를 만들고 디버거가
붙습니다. 중단점을 걸고 변수를 들여다볼 수 있습니다.

### 파일 하나만 실행·디버그하기

**C 디버그 (현재 파일)**은 열려 있는 `.c` 파일을 그대로 디버깅합니다. 폴더가
늘어나도 구성을 새로 만들 필요가 없습니다.

같은 폴더의 `.c`를 함께 링크하므로, 구현이 옆 파일에 있어도 됩니다. 대신
**한 폴더에 `main`은 하나만** 두세요.

터미널에서 직접 부를 수도 있습니다.

```sh
make src/main.debug.out && ./src/main.debug.out
```

### ▶ 버튼에 대해

편집기 오른쪽 위의 ▶ 버튼은 **Code Runner** 확장이 제공합니다. C든 Python이든
열려 있는 파일을 그대로 실행합니다.

두 확장이 각각 ▶ 버튼을 내놓으면 헷갈리므로, C/C++ 확장 쪽은 꺼 두었습니다
(`C_Cpp.debugShortcut`). 그쪽 버튼은 **파일 하나만** 컴파일해서 이런 오류를
냅니다.

```console
undefined reference to `selectionSort'
collect2: error: ld returned 1 exit status
```

Code Runner도 기본 설정 그대로면 같은 문제가 나고, Python은 이미지에 없는
`python`을 찾습니다. 그래서 `.vscode/settings.json`에서 두 가지를 고쳐
두었습니다.

- C는 `Makefile`의 `%.out` 규칙을 거쳐 **같은 폴더의 `.c`를 함께** 빌드합니다
- Python은 `python3`로 실행합니다
- 출력 패널이 아니라 **터미널**에서 돌립니다. 그래야 `scanf`나 `input()`이 멈추지 않습니다

## 저장소 구조

```plaintext
algorithm_hw1/
├── .devcontainer/devcontainer.json  # Codespaces · Dev Containers 설정
├── compose.yml                      # 실습 컨테이너 (서비스 이름: lab)
├── Dockerfile                       # gcc · gdb · make · python3 · git
├── .vscode/                         # 빌드·디버그 설정 (F5, Cmd+Shift+B)
├── Makefile                         # run · test · bench · charts · debug · clean
├── src/
│   ├── sort.h · sort.c              # 공통 인터페이스와 정렬 표
│   ├── selectionSort.c              # 선택 정렬
│   ├── shellSort.c                  # 셸 정렬 (gap = n/2, n/4, ..., 1)
│   ├── treeSort.c                   # 트리 정렬 (이진 탐색 트리 + 중위 순회)
│   ├── bench.h · bench.c            # 입력 생성과 시간 측정
│   ├── main.c                       # 예제 실행 · --bench · --csv
│   ├── sort.py                      # Python 구현 (같은 알고리즘, 같은 방식으로 센다)
│   └── main.py                      # Python 예제 실행 · 안정성 실측
├── tests/
│   ├── test_sort.c                  # C 유닛 테스트 (표준 C만 사용)
│   └── test_sort.py                 # Python 유닛 테스트 (unittest)
├── tools/plot.py · svgchart.py      # CSV → SVG 그래프 (표준 모듈만)
└── report/                          # 보고서 · 측정 원본(results.csv) · 그래프
```

## 규약

- **실행 파일은 `*.out`으로 만듭니다.** `.gitignore`가 `*.out`만 걸러내므로,
  컨테이너에서 컴파일한 Linux 바이너리가 커밋에 섞이지 않습니다.
- **외부 라이브러리를 쓰지 않습니다.** C는 표준 라이브러리만, Python은 표준
  모듈만 씁니다. C 테스트도 프레임워크 없이 `assert` 수준으로 직접 씁니다.
- **C와 Python은 같은 알고리즘을 같은 이름의 함수로 구현합니다.** 언어 차이가
  알고리즘 차이로 보이지 않게 합니다.
- 파일명은 각 언어의 관례를 따릅니다. C는 camelCase(`treeSort`), Python은
  snake_case(`tree_sort`)입니다.

## 변경 기록

버전과 변경 내역은 [CHANGELOG.md](CHANGELOG.md)에 있습니다.

## 정리

```sh
docker compose down
```

컨테이너를 지워도 코드는 그대로 남습니다.
