from typing import Type, Dict
from ..tsl.tsl import TSL
from .swig import swig_methods_generate


def _get_type_name(t: 'Type'):
    return t.__name__


# generate the tsl type for only once

def def_tsl(class_def: Type):
    """
    conventions
        - add a meta private field `__tsl_src__` for an instance of `class_def`.
            to represent the data pointer.
    """
    annotations: 'Dict[str, type]' = class_def.__annotations__

    signature = (class_def.__name__,
                 *sorted(
                     map(_get_type_name, annotations.values())))

    if signature in TSL.types:
        return TSL.types[signature]

    TSL.register_type(signature, class_def)

    class_def.__dict__.update(
        swig_methods_generate(class_def.__name__,
                              annotations))

    return class_def
