# -*- coding: utf-8 -*-
"""
setup.py - AstPy Python Package Configuration

Usage:
    pip install .
    pip install . --editable  # Development install
    python setup.py build_ext --inplace
"""

import os
import sys
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

# Version
VERSION = "1.0.0"

# Package metadata
PACKAGES = ["astpy"]
PACKAGE_DIR = {"astpy": "examples/python/astpy"}


class CMakeBuildExt(build_ext):
    """Custom build extension using CMake."""
    
    def build_extensions(self):
        # For simplicity, we skip CMake-based build here
        # In production, use scikit-build or pybind11/cmake
        pass


# Read README for long description
readme_path = os.path.join(os.path.dirname(__file__), "src", "AstPy", "README.md")
if os.path.exists(readme_path):
    with open(readme_path, encoding="utf-8") as f:
        long_description = f.read()
else:
    long_description = "SpaceAST Python Bindings"


setup(
    name="astpy",
    version=VERSION,
    author="SpaceAST Development Team",
    author_email="space-ast@example.com",
    description="Python bindings for SpaceAST astrodynamics library",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://gitee.com/space-ast/ast",
    license="Apache License 2.0",
    packages=find_packages(where="examples/python"),
    package_dir={"astpy": "examples/python/astpy"},
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Scientific/Engineering :: Astronomy",
        "Topic :: Scientific/Engineering :: Physics",
    ],
    python_requires=">=3.7",
    install_requires=[
        "numpy>=1.19.0",
    ],
    extras_require={
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "astpy-example=examples.python.basic_usage:main",
        ],
    },
)


# Build instructions for manual compilation:
# ==========================================
#
# Prerequisites:
#   - Python 3.7+
#   - pybind11
#   - SpaceAST C++ library
#   - CMake 3.15+ or xmake
#
# Building with CMake:
#   mkdir build && cd build
#   cmake .. -DAST_BUILD_PYTHON=ON -DCMAKE_BUILD_TYPE=Release \
#            -DCMAKE_PREFIX_PATH=/path/to/ast \
#            -DPYTHON_EXECUTABLE=$(which python)
#   cmake --build . --config Release
#   cmake --install .
#
# Building with xmake:
#   xmake f -m release
#   xmake
#   xmake install
#
# Manual build:
#   c++ -O3 -shared -std=c++17 -fPIC \
#       $(python3 -m pybind11 --includes) \
#       -I./src -I./thirdparty/pybind11/include \
#       src/AstPy/module.cpp src/AstPy/*.cpp \
#       -L./build/lib -lAstCore -lAstMath -lAstUtil \
#       -o examples/python/astpy/_ast*.so
