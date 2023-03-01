# flake8:noqa:F841

import pytest

from modint.modint import ModInt
from modint.modint_pure import using_modint

modintP = using_modint(998244353)


class TestPure:
    def test_init(self) -> None:
        a = modintP()
        a = modintP(5)

    def test_slots(self) -> None:
        a = modintP(3)
        with pytest.raises(AttributeError):
            a.__dict__

    def test_property(self) -> None:
        a = modintP(4)
        assert a.value == 4

        with pytest.raises(AttributeError):
            a.value = 10  # type:ignore

    def test_add(self) -> None:
        a = modintP(3)
        b = modintP(4)
        c = a + b
        assert c.value == 7

    def test_iadd(self) -> None:
        a = modintP(3)
        b = modintP(4)
        a += b
        assert a.value == 7

    def test_mutable(self) -> None:
        l = [modintP()] * 2
        assert id(l[0]) == id(l[1])
        l[0] += 5
        assert id(l[0]) == id(l[1])

    def test_diff_mod(self) -> None:
        mint3 = using_modint(3)
        mint7 = using_modint(7)
        a = mint3(2)
        b = mint7(3)
        with pytest.raises(ValueError):
            a + b

    def test_diff_multi_using(self) -> None:
        mint_a = using_modint(7)
        mint_b = using_modint(7)

        a = mint_a(3)
        b = mint_b(5)

        a + b


class TestCExt:
    def test_init(self) -> None:
        a = ModInt()
        a = ModInt(5)

    def test_property(self) -> None:
        a = ModInt(4)
        assert a.value == 4

        with pytest.raises(AttributeError):
            a.value = 10  # type:ignore

    def test_add(self) -> None:
        a = ModInt(3)
        b = ModInt(4)
        c = a + b
        assert c.value == 7

    def test_iadd(self) -> None:
        a = ModInt(3)
        b = ModInt(4)
        a += b
        assert a.value == 7

    def test_add_int(self) -> None:
        a = ModInt(3)
        b = a + 4
        assert b.value == 7

    def test_immutable(self) -> None:
        l = [ModInt()] * 2
        assert id(l[0]) == id(l[1])
        l[0] += 5
        assert id(l[0]) != id(l[1])
