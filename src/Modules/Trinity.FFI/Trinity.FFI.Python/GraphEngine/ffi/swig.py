from typing import Callable, Dict
from .ffi import *
from ..tsl.tsl import TSL

_swig_method_generate: 'import from ffi'

_grouping_characteristic = '蛤'
LegalName = str  # identifier without '蛤'
MaybeILLegalName = str  # identifier with '蛤' maybe


def naming_rule(type_name, field_name, field_type_name, operation):
    def legalise(name: MaybeILLegalName):
        return name.replace(_grouping_characteristic, _grouping_characteristic * 2)

    return _grouping_characteristic.join(map
                                         (legalise,
                                          [type_name, field_name, field_type_name, operation]))


def swig_methods_generate(type_name, fields_sig: 'Dict[str, type]') -> 'Dict[str, Callable]':
    # TODO generic type???
    """
    implemented at .NET end.

    a lazy method should follow these rules:
        - do not call code-gen and swig build tools if it hasn't been used.
        - once being called, do code-gen and build the corresponding swig files and
          extract out the method, call it with the arguments in current context once,
          and the replace the lazy method with the generated one.
    very interesing zero cost abstraction in Python end :)
    """

    def swig_method_generate_getter_setter(type_name, field_name, field_type_name):
        get = _swig_method_generate(type_name, field_name, field_type_name, 'get')
        set = _swig_method_generate(type_name, field_name, field_type_name, 'set')

        @property
        def agent(self):
            return get(self.__tsl_src__)

        @agent.setter
        def agent(self, value):
            return set(self.__tsl_src__, value)

        return agent

    return {field_name:
                swig_method_generate_getter_setter(type_name, field_name, field_type.__name__) for
            field_name, field_type in fields_sig.items()}
