# modint

## これは何？

[atcoder::dynamic_modint](https://atcoder.github.io/ac-library/document_ja/modint.html)をpythonで再現したモジュール。

## 目標

* atcoder::dynamic_modintと同等の機能
* intと同程度の処理時間

## 使用例のイメージ

[Educational DP Contest H - Grid 1](https://atcoder.jp/contests/dp/tasks/dp_h)

```py
from modint import UsingModInt

MOD = 1000000007
h, w = map(int, input().split())
a = [input() for _ in range(h)]

Mint = UsingModInt(MOD)
dp = [[Mint()] * w for _ in range(h)]
dp[0][0] = Mint(1)
for i in range(h):
    for j in range(w):
        if a[i][j] == "#":
            continue
        if i != 0:
            dp[i][j] += dp[i - 1][j]
        if j != 0:
            dp[i][j] += dp[i][j - 1]

print(dp[-1][-1])

```

## 動作環境
* long型のサイズが8byte(=64ビット環境)であること。
