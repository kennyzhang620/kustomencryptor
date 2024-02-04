from setuptools import setup
from Cython.Build import cythonize

setup(
    name='kencrypt',
    ext_modules=cythonize("kencrypt.pyx"),
)

setup(
    name='filelib',
    ext_modules=cythonize("ksplitter.pyx"),
)