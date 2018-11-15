import os
from distutils.core import setup
from Cython.Build import cythonize

os.environ['CFLAGS'] = '-std=c++14 -stdlib=libc++ -Wall'

setup(ext_modules=cythonize("PyKVOStore.pyx", language="c++"))
