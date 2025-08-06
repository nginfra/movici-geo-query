"""Build configuration for movici-geo-query C++ extensions."""

import os
import sys
from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup

# Compiling requires Boost header files. The compiler can try and find Boost at the usual locations
# Alternatively the BOOST_DIR environment variable can be set, point to the directory with
# the Boost header files
BOOST_DIR = Path(os.environ.get("BOOST_DIR", "")).absolute().parent

CURRENT_DIR = Path(__file__).parent
SRC_DIR = CURRENT_DIR / "src"

_DEBUG = False
_DEBUG_LEVEL = 0
if sys.platform.startswith("win32"):
    extra_compile_args = []
    extra_link_args = []
else:
    # Use Python's exact compiler configuration for ABI compatibility
    import sysconfig

    cflags = sysconfig.get_config_var("CFLAGS") or ""
    ldflags = sysconfig.get_config_var("LDFLAGS") or ""

    extra_compile_args = ["-Wall", "-Wextra", "-std=c++17"]
    # Add Python's flags for ABI compatibility
    extra_compile_args.extend(["-fno-strict-overflow", "-fPIC"])

    extra_link_args = []
    # Add Python's linking flags
    if ldflags:
        extra_link_args.extend(ldflags.split())

    if _DEBUG:
        extra_compile_args += ["-g3", "-O0", f"-DDEBUG={_DEBUG_LEVEL}", "-UNDEBUG"]
    else:
        extra_compile_args += ["-DNDEBUG", "-O3"]

ext_modules = [
    Pybind11Extension(
        "_movici_geo_query",
        sorted(str(file.relative_to(CURRENT_DIR)) for file in SRC_DIR.glob("*.cpp")),
        include_dirs=[str(SRC_DIR), str(BOOST_DIR)],
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),
]

# Minimal setup - all metadata is now in pyproject.toml
setup(
    ext_modules=ext_modules,
)
