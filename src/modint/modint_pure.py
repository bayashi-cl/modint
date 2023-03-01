"""modint_pure module
"""

from __future__ import annotations

import functools

try:
    from typing import Self
except ImportError:
    from typing_extensions import Self


class _modint_base:
    """_summary_

    :raises ValueError: _description_
    :raises ValueError: _description_
    :return: _description_
    :rtype: _type_
    """

    mod: int = 0

    __slots__ = ("_value",)

    def __init__(self: Self, v: int = 0) -> None:
        self._value = v % self.mod

    @property
    def value(self: Self) -> int:
        return self._value

    def __add__(self: Self, rhs: int | Self) -> Self:
        if isinstance(rhs, type(self)):
            return type(self)(self._value + rhs._value)
        elif isinstance(rhs, int):
            return type(self)(self._value + rhs)
        else:
            raise ValueError

    def __iadd__(self: Self, rhs: int | Self) -> Self:
        if isinstance(rhs, type(self)):
            self._value = (self._value + rhs._value) % self.mod
        elif isinstance(rhs, int):
            self._value = (self._value + rhs) % self.mod
        else:
            raise ValueError

        return self


@functools.cache
def using_modint(mod: int) -> type[_modint_base]:
    """using_modint _summary_

    :param mod: _description_
    :type mod: int
    :return: _description_
    :rtype: type[_modint_base]
    """
    return type(f"modint{mod}", (_modint_base,), {"__slots__": (), "mod": mod})
