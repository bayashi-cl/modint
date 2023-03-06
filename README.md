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

## ベンチマーク

```
--------------- benchmark 'testcase=EDPC_H(MOD=1000000007, h=1000, w=1000)': 3 tests --------------
Name (time in ms)                             Min                 Max                Mean
---------------------------------------------------------------------------------------------------
test_benchmark[cext-testcase0]           245.8627 (1.0)      253.2607 (1.0)      249.1922 (1.0)
test_benchmark[pure_hand-testcase0]      278.9648 (1.13)     306.7191 (1.21)     286.5652 (1.15)
test_benchmark[pure_class-testcase0]     749.6728 (3.05)     809.5896 (3.20)     779.7499 (3.13)
---------------------------------------------------------------------------------------------------

----------------- benchmark 'testcase=EDPC_H(MOD=1000000007, h=20, w=10)': 3 tests ----------------
Name (time in us)                            Min                   Max               Mean
---------------------------------------------------------------------------------------------------
test_benchmark[cext-testcase1]           39.9000 (1.0)        354.1000 (1.0)      44.3919 (1.0)
test_benchmark[pure_hand-testcase1]      41.2000 (1.03)     4,710.2010 (13.30)    45.0914 (1.02)
test_benchmark[pure_class-testcase1]     87.5000 (2.19)       485.3000 (1.37)     97.6120 (2.20)
---------------------------------------------------------------------------------------------------
```

## 動作環境
* long型のサイズが8byte(=64ビット環境)であること。
