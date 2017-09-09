from distutils.core import setup, Extension

graphengine_module = Extension('GEKV',
                           sources = ['GEKV.cpp'],
                           include_dirs=['../../../Trinity.C/include', '../../../Trinity.C/src'],
                           library_dirs=['../../../../bin'],
                           libraries=['Trinity.C'])

setup(name = 'GEKV',
      version = '0.1',
      description = 'A Python module for Graph Engine in-memory Key-Value store.',
      ext_modules = [graphengine_module],
      url='https://github.com/Microsoft/GraphEngine',
      author='Graph Engine team',
      author_email='graph@microsoft.com')