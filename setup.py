import os
import sys
from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension
from setuptools import find_packages, setup

# Compiling requires Boost. The compiler can try and find Boost at the usual locations
# Alternatively the BOOST_DIR environment variable can be set, point to the directory with
# the Boost header files
BOOST_DIR = Path(os.environ.get("BOOST_DIR", "")).absolute().parent

def read_file_or_empty_str(file, comment_tag=None):
    try:
        with open(file) as fh:
            if comment_tag is not None:
                return "\n".join(
                    r.strip("\n")
                    for r in fh.readlines()
                    if not r.startswith(comment_tag)
                )
            return fh.read()
    except FileNotFoundError:
        return ""


REQUIREMENTS = read_file_or_empty_str("requirements.txt")
README = read_file_or_empty_str("README.md")
LICENSE = read_file_or_empty_str("LICENSE")
VERSION = read_file_or_empty_str("VERSION", comment_tag="#")
CURRENT_DIR = Path(__file__).parent
SRC_DIR = CURRENT_DIR / "src"


_DEBUG = False
_DEBUG_LEVEL = 0
if sys.platform.startswith("win32"):
    extra_compile_args = []
else:
    extra_compile_args = ["-Wall", "-Wextra", "-std=c++17"]
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
    ),
]

setup(
    name="movici-geo-query",
    version=VERSION,
    description="Geospatial queries powered by Boost Geom",
    long_description=README,
    ext_modules=ext_modules,
    author="Eelco Naarding",
    author_email="eelco.naarding@gmail.com",
    license=LICENSE,
    packages=find_packages(),
    install_requires=REQUIREMENTS,
)
