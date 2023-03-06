from __future__ import annotations


def __py_mod(value: int, mod: int) -> int:
    return value % mod


def __ModInt_new_raw(value: int, mod: int) -> ModInt:
    self = ModInt.__new__(ModInt)
    self._value = value
    self._mod = mod
    return self


class ModInt:
    def __init__(self, value: int, mod: int) -> None:
        if mod <= 0:
            raise ValueError("mod is not positive.")
        if mod > 4294967295:
            raise ValueError("mod exceeds UINT32_MAX.")
        self._value = __py_mod(value, mod)
        self._mod = mod

    # @property
    # def value(self) -> int:
    #     return self._value

    # @property
    # def mod(self) -> int:
    #     return self._mod

    def inv(self) -> ModInt:
        return ModInt(pow(self._value, -1, self._mod), self._mod)

    def __add__(self, rhs: int | ModInt) -> ModInt:
        value = 0
        mod = self._mod
        if isinstance(rhs, ModInt):
            if self._mod != rhs._mod:
                raise ValueError(f"{self._mod=} differs from {rhs._mod=}.")
            value = self._value + rhs._value
        elif isinstance(rhs, int):
            value = self._value + __py_mod(rhs, mod)
        else:
            raise TypeError("operator is only available between int and ModInt.")

        if value >= mod:
            value -= mod
        return __ModInt_new_raw(value, mod)

    def __radd__(self, lhs: int) -> ModInt:
        value = 0
        mod = self._mod
        if isinstance(lhs, int):
            value = __py_mod(lhs, mod) + self._value
        else:
            raise TypeError("operator is only available between int and ModInt.")
        return __ModInt_new_raw(value, mod)

    def __iadd__(self, rhs: int | ModInt) -> ModInt:
        self = self.__add__(rhs)
        return self

    def __sub__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __rsub__(self, lhs: int) -> ModInt:
        return NotImplemented

    def __isub__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __mul__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __rmul__(self, lhs: int) -> ModInt:
        return NotImplemented

    def __imul__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __pow__(self, rhs: int | ModInt, mod: int) -> ModInt:
        return NotImplemented

    def __neg__(self) -> ModInt:
        return NotImplemented

    def __pos__(self) -> ModInt:
        return NotImplemented

    def __abs__(self) -> ModInt:
        return NotImplemented

    def __bool__(self) -> bool:
        return NotImplemented

    def __int__(self) -> int:
        return NotImplemented

    def __floordiv__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __rfloordiv__(self, lhs: int) -> ModInt:
        return NotImplemented

    def __ifloordiv__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __truediv__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented

    def __rtruediv__(self, lhs: int) -> ModInt:
        return NotImplemented

    def __itruediv__(self, rhs: int | ModInt) -> ModInt:
        return NotImplemented


class UsingModInt:
    def __init__(self, mod: int) -> None:
        if mod <= 0:
            raise ValueError("mod is not positive.")
        if mod > 4294967295:
            raise ValueError("mod exceeds UINT32_MAX.")
        self._mod = mod

    @property
    def mod(self) -> int:
        return self._mod

    def __call__(self, value: int = 0) -> ModInt:
        return ModInt(value, self.mod)
