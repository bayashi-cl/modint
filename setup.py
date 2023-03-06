import ctypes
import sys

from setuptools import Extension, find_packages, setup

if ctypes.sizeof(ctypes.c_long) != 8:
    sys.exit(-1)

setup(
    name="modint",
    description="modint for Python.",
    author="bayashi-cl",
    package_dir={"": "src"},
    packages=find_packages("src"),
    package_data={"": ["*.pyi", "*.cpp", "py.typed"]},
    python_requires=">3.9",
    install_requires=["typing_extensions"],
    extras_require={
        "dev": [
            "pytest",
            "pytest-benchmark",
            "flake8",
            "mypy",
            "black",
        ],
        "doc": [
            "Sphinx",
            "sphinx-rtd-theme",
        ],
    },
    ext_modules=[
        Extension(
            "modint._modint",
            ["src/modint/_modint.cpp"],
            extra_compile_args=["-std=c++2a"],
        ),
    ],
)
