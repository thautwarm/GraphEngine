#include "Python.h"
#include "Trinity/Python/GraphEngine.h"
#include "TrinityCommon.h"

static PyObject* initialize(PyObject *self, PyObject *args)
{
    const char * storage_root = "E:\\sandbox\\TempStorageRoot";
    gpy_initialize(storage_root, 64);
	Py_RETURN_NONE;
}

static PyMethodDef graphengine_module_methods[] = {
	{
		"initialize",
		initialize,
		METH_NOARGS,
		"Initialize Graph Engine."
	},
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef graphengine_module_definition = {
	PyModuleDef_HEAD_INIT,
	"graphengine_module",
	"Graph Engine for Python.",
	-1,
	graphengine_module_methods
};

PyMODINIT_FUNC PyInit_GraphEngine(void)
{
	Py_Initialize();
	return PyModule_Create(&graphengine_module_definition);
}