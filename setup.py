"""Build configuration for movici-geo-query C++ extensions."""

import os
import sys
import tarfile
import urllib.request
from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup

CURRENT_DIR = Path(__file__).parent
SRC_DIR = CURRENT_DIR / "src"

# Compiling requires Boost header files. The compiler can try and find Boost at the usual locations
# Alternatively the BOOST_DIR environment variable can be set, point to the directory with
# the Boost header files
boost_env = os.environ.get("BOOST_DIR", "")
if boost_env:
    BOOST_DIR = Path(boost_env).absolute()
else:
    # Try common system locations for Boost
    common_boost_paths = [
        "/usr/include",
        "/usr/local/include",
        "/opt/homebrew/include",  # macOS with Homebrew
    ]
    BOOST_DIR = None
    for path in common_boost_paths:
        boost_path = Path(path)
        if (boost_path / "boost").exists():
            BOOST_DIR = boost_path
            break

    if BOOST_DIR is None:
        # Fallback to current directory - CMake might have set up boost headers
        BOOST_DIR = CURRENT_DIR

# Validate that boost headers actually exist at the detected location
if BOOST_DIR:
    boost_header_check = BOOST_DIR / "boost" / "geometry.hpp"
    if not boost_header_check.exists():
        print(f"Warning: boost/geometry.hpp not found at {boost_header_check}")
        print("Will download Boost headers via FetchContent approach")
        BOOST_DIR = None

# Download Boost headers if not available on system (same as CMake FetchContent)
if BOOST_DIR is None:
    print("Downloading Boost 1.84.0 headers for compilation...")
    boost_build_dir = CURRENT_DIR / "build_boost"
    boost_extract_dir = boost_build_dir / "boost-1.84.0"

    if not boost_extract_dir.exists():
        boost_build_dir.mkdir(exist_ok=True)
        boost_url = (
            "https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.gz"
        )
        boost_archive = boost_build_dir / "boost-1.84.0.tar.gz"

        print(f"Downloading {boost_url}...")
        urllib.request.urlretrieve(boost_url, boost_archive)  # noqa: S310

        print("Extracting Boost archive...")
        with tarfile.open(boost_archive, "r:gz") as tar:
            tar.extractall(boost_build_dir)  # noqa: S202

        # Clean up archive
        boost_archive.unlink()

    # Set up include directories like CMake does
    boost_include_dirs = [str(boost_extract_dir)]

    # Add all libs/*/include directories
    libs_dir = boost_extract_dir / "libs"
    if libs_dir.exists():
        for lib_dir in libs_dir.iterdir():
            if lib_dir.is_dir():
                include_dir = lib_dir / "include"
                if include_dir.exists():
                    boost_include_dirs.append(str(include_dir))

    # Special handling for numeric conversion headers (same as CMakeLists.txt)
    numeric_conversion_include = boost_extract_dir / "libs" / "numeric" / "conversion" / "include"
    if numeric_conversion_include.exists():
        boost_include_dirs.append(str(numeric_conversion_include))

    print(f"Using downloaded Boost with {len(boost_include_dirs)} include directories")
else:
    boost_include_dirs = [str(BOOST_DIR)]

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

# Build include directories list
include_dirs = [str(SRC_DIR)] + boost_include_dirs

ext_modules = [
    Pybind11Extension(
        "_movici_geo_query",
        sorted(str(file.relative_to(CURRENT_DIR)) for file in SRC_DIR.glob("*.cpp")),
        include_dirs=include_dirs,
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),
]

# Minimal setup - all metadata is now in pyproject.toml
setup(
    ext_modules=ext_modules,
)
