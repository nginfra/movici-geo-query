from setuptools import setup, find_packages


def read_file_or_empty_str(file):
    try:
        with open(file) as f:
            return f.read()
    except FileNotFoundError:
        return ""


REQUIREMENTS = read_file_or_empty_str("requirements.txt")
README = read_file_or_empty_str("README.md")
LICENSE = read_file_or_empty_str("LICENSE")
VERSION = read_file_or_empty_str("VERSION")

setup(
    name="boost_geo_query",
    version=VERSION,
    description="Boost geospatial queries in C++",
    long_description=README,
    author="Eelco Naarding",
    author_email="eelco.naarding@gmail.com",
    license=LICENSE,
    packages=find_packages(),
    install_requires=REQUIREMENTS,
)
