import pytest

from .problem import EDPC_H, edpc_h_cases


def cext(*, MOD: int, h: int, w: int, a: list[str], **kwargs):
    from modint.modint import ModInt

    ModInt.set_mod(MOD)

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

    return dp[-1][-1].value


def cext_multi(*, MOD: int, h: int, w: int, a: list[str], **kwargs):
    from modint.modint_multi import UsingModInt

    mint = UsingModInt(MOD)

    dp = [[mint()] * w for _ in range(h)]
    dp[0][0] = mint(1)
    for i in range(h):
        for j in range(w):
            if a[i][j] == "#":
                continue
            if i + 1 != h and a[i + 1][j] == ".":
                dp[i + 1][j] += dp[i][j]
            if j + 1 != w and a[i][j + 1] == ".":
                dp[i][j + 1] += dp[i][j]

    return dp[-1][-1].value


def pure_class(*, MOD: int, h: int, w: int, a: list[str], **kwargs):
    from modint.modint_pure import using_modint

    mint = using_modint(MOD)

    dp = [[mint() for _ in range(w)] for _ in range(h)]
    dp[0][0] = mint(1)
    for i in range(h):
        for j in range(w):
            if a[i][j] == "#":
                continue
            if i + 1 != h and a[i + 1][j] == ".":
                dp[i + 1][j] += dp[i][j]
            if j + 1 != w and a[i][j + 1] == ".":
                dp[i][j + 1] += dp[i][j]

    return dp[-1][-1].value


def pure_hand(*, MOD: int, h: int, w: int, a: list[str], **kwargs):
    dp = [[0] * w for _ in range(h)]
    dp[0][0] = 1
    for i in range(h):
        for j in range(w):
            if a[i][j] == "#":
                continue
            if i + 1 != h and a[i + 1][j] == ".":
                dp[i + 1][j] = (dp[i + 1][j] + dp[i][j]) % MOD
            if j + 1 != w and a[i][j + 1] == ".":
                dp[i][j + 1] = (dp[i][j + 1] + dp[i][j]) % MOD

    return dp[-1][-1]


def idfun(val):
    if callable(val):
        return val.__name__
    elif isinstance(val, EDPC_H):
        return "edpc"
    else:
        raise ValueError


@pytest.mark.parametrize("testcase", edpc_h_cases)
@pytest.mark.parametrize(
    "solver",
    [
        cext,
        cext_multi,
        pure_hand,
        # pure_class,
    ],
)
def test_benchmark(benchmark, solver, testcase: EDPC_H) -> None:
    assert benchmark(solver, **testcase._asdict()) == testcase.answer
