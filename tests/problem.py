from math import comb
from typing import Final, NamedTuple


def ncr(n: int, r: int, mod: int):
    r = min(r, n - r)
    assert r < mod
    num = 1
    den = 1
    for i in range(r):
        num = num * (n - i) % mod
        den = den * (i + 1) % mod

    return num * pow(den, -1, mod) % mod


class EDPC_H(NamedTuple):
    MOD: int
    h: int
    w: int
    a: list[str]
    answer: int

    def __repr__(self) -> str:
        return f"EDPC_H(MOD={self.MOD}, h={self.h}, w={self.w})"


def gen_empty_case(h: int, w: int, mod: int) -> EDPC_H:
    return EDPC_H(
        MOD=mod,
        h=h,
        w=w,
        a=["." * w] * h,
        answer=ncr((h - 1) + (w - 1), (h - 1), mod),
    )


edpc_h_cases = [
    gen_empty_case(1000, 1000, 1000000007),
    gen_empty_case(20, 10, 1000000007),
]


if __name__ == "__main__":
    mod = 998244353
    assert ncr(10, 5, mod) == comb(10, 5)
    assert ncr(314, 159, mod) == comb(314, 159) % mod
