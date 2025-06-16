from setuptools import setup, Extension

module = Extension('cjson', sources=['cjson.c'])

setup(
    name='cjson',
    version='1.0',
    description='JSON parser and dumper in C++',
    ext_modules=[module],
)
