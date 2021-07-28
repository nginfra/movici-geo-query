from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup, find_packages


def read_file_or_empty_str(file, comment_tag=None):
    try:
        with open(file) as fh:
            if comment_tag is not None:
                return "\n".join(
                    r.strip("\n") for r in fh.readlines() if not r.startswith(comment_tag)
                )
            return fh.read()
    except FileNotFoundError:
        return ""


REQUIREMENTS = read_file_or_empty_str("requirements.txt")
README = read_file_or_empty_str("README.md")
LICENSE = read_file_or_empty_str("LICENSE")
VERSION = read_file_or_empty_str("VERSION", comment_tag='#')
CURRENT_DIR = Path(__file__).parent
SRC_DIR = CURRENT_DIR / "src"
_DEBUG = False
_DEBUG_LEVEL = 0
# extra_compile_args = sysconfig.get_config_var('CFLAGS').split()
extra_compile_args = ["-Wall", "-Wextra", "-std=c++17"]
if _DEBUG:
    extra_compile_args += ["-g3", "-O0", "-DDEBUG=%s" % _DEBUG_LEVEL, "-UNDEBUG"]
else:
    extra_compile_args += ["-DNDEBUG", "-O3"]

ext_modules = [
    Pybind11Extension(
        "interface",
        sorted(str(file) for file in SRC_DIR.glob("*.cpp")),
        include_dirs=[str(SRC_DIR)],
        extra_compile_args=extra_compile_args,
    ),
]

setup(
    name="boost-geo-query",
    version=VERSION,
    description="Boost geospatial queries in C++",
    long_description=README,
    ext_modules=ext_modules,
    author="Eelco Naarding",
    author_email="eelco.naarding@gmail.com",
    license=LICENSE,
    packages=find_packages(),
    install_requires=REQUIREMENTS,
)
