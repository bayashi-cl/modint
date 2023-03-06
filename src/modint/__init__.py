import sys

try:
    from ._modint import UsingModInt
except ImportError:
    print("pure module used", file=sys.stderr)
    from ._modint_pure import UsingModInt

__all__ = ["UsingModInt"]
