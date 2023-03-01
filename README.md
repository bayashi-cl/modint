# modint

## これは何？

[atcoder::dynamic_modint](https://atcoder.github.io/ac-library/document_ja/modint.html)をpythonで再現したモジュール。

## 目標

* atcoder::dynamic_modintと同等の機能
* intと同程度の処理時間

## 使用例のイメージ

[Educational DP Contest H - Grid 1](https://atcoder.jp/contests/dp/tasks/dp_h)

```py
from modint.modint import ModInt

ModInt.set_mod(1000000007)
h, w = map(int, input().split())
a = [input() for _ in range(h)]

dp = [[ModInt()] * w for _ in range(h)]
dp[0][0] = ModInt(1)
for i in range(h):
    for j in range(w):
        if a[i][j] == "#":
            continue
        if i + 1 != h and a[i + 1][j] == ".":
            dp[i + 1][j] += dp[i][j]
        if j + 1 != w and a[i][j + 1] == ".":
            dp[i][j + 1] += dp[i][j]

print(dp[-1][-1].value)

```
