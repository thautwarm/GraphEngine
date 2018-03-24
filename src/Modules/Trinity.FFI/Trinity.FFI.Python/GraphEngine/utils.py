class _ClassProperty:
    def __init__(self, method):
        self.method = method

    def __get__(self, _, instance_cls):
        return self.method(instance_cls)


class_property = _ClassProperty
