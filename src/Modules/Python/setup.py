from distutils.core import setup, Extension

graphengine_module = Extension('GraphEngine',
                           sources = ['GraphEngine.cpp'],
                           include_dirs=['../../Trinity.C/include', '../../Trinity.C/src'],
                           library_dirs=['../../../bin'],
                           libraries=['Trinity.C'])

setup(name = 'GraphEngine',
      version = '0.1',
      description = 'A Python module for Graph Engine.',
      ext_modules = [graphengine_module],
      url='https://github.com/Microsoft/GraphEngine',
      author='Graph Engine team',
      author_email='graph@microsoft.com')