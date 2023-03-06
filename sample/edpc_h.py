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
