from ..utils import class_property
from enum import Enum
from abc import ABC, abstractmethod, abstractclassmethod
from typing import Union, TypeVar, Type, Callable

if False:
    from Cython.Shadow import void

Basic = Union[int, float, str]
T = TypeVar('T')


class TSLType(ABC):
    __tsl_src__: "void*"

    @abstractclassmethod
    def __init__(self, *args):
        """
        >>> new_cell = Cell()
        >>> new_cell = Cell(1, [1, 2, 3])
        init with  ordered parameters or no parameter.
        """
        pass

    @abstractclassmethod
    def new(self, **kwargs) -> 'TSLType':
        """
        >>> new_cell = Cell.new(attr1=1, attr2=[1, 2, 3])
        """
        pass

    @classmethod
    def from_pointer(cls: 'Union[Type[T], Callable]', src) -> 'T':
        """
        >>> new_cell = Cell.from_pointer(src)
        """
        new = cls
        new.__tsl_src__ = src
        return new


class TSL:
    _type_storage = {}

    @class_property
    def types(cls) -> 'dict':
        return cls._type_storage

    @classmethod
    def register_type(cls, sig, typ):
        cls._type_storage[sig] = typ

    @classmethod
    def type_map(cls, type_name, src) -> 'Union[T, Basic]':
        """
        in Python end, tsl type is also a constructor.
        """
        tsl_type: 'Union[TSLType, Type[T], Callable]' = cls._type_storage.get(type_name)
        if not tsl_type:
            return src
        return tsl_type.from_pointer(src)
