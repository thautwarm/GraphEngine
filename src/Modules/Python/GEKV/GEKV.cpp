#include "Python.h"
#include "Trinity/Python/GEKV.h"
#include "TrinityCommon.h"

static PyObject* init_gekv(PyObject *self, PyObject *args)
{
	const char * storage_root;
	int TrunkCount;
    if (!PyArg_ParseTuple(args, "si", &storage_root, &TrunkCount))
        return NULL;
	return Py_BuildValue("i", gpy_initialize(storage_root, TrunkCount));
}

static PyObject* save_cell(PyObject *self, PyObject *args)
{
	long long cellId;
	char * buff;
	int size;
	unsigned short int cellType;
	if (!PyArg_ParseTuple(args, "LsiH", &cellId, &buff, &size, &cellType))
		return NULL;
    return Py_BuildValue("i", gpy_save_cell(cellId, buff, size, cellType));
}

static PyObject* update_cell(PyObject *self, PyObject *args)
{
	long long cellId;
	char * buff;
	int size;
	if (!PyArg_ParseTuple(args, "Lsi", &cellId, &buff, &size))
		return NULL;
    return Py_BuildValue("i", gpy_update_cell(cellId, buff, size));
}

static PyObject* remove_cell(PyObject *self, PyObject *args)
{
	long long cellId;
	if (!PyArg_ParseTuple(args, "L", &cellId))
		return NULL;
    return Py_BuildValue("i", gpy_remove_cell(cellId));
}

static PyObject* load_cell(PyObject *self, PyObject *args)
{
	long long cellId;
	if (!PyArg_ParseTuple(args, "L", &cellId))
		return NULL;
	int32_t size;
	uint16_t type;
	char* cellPtr;
	int32_t entryIndex;	
	int32_t error_code = gpy_get_locked_cell_info(cellId, size, type, cellPtr, entryIndex);
	if(error_code == TrinityErrorCode::E_CELL_NOT_FOUND)
		return Py_BuildValue("i", error_code);
	PyObject* ret = Py_BuildValue("y#", cellPtr, size);
	gpy_release_cell_lock(cellId, entryIndex);
	return ret;
}

static PyObject* save_storage(PyObject *self, PyObject *args)
{
	gpy_save_storage();
	Py_RETURN_NONE;
}

static PyObject* load_storage(PyObject *self, PyObject *args)
{
	gpy_load_storage();
	Py_RETURN_NONE;
}

static PyObject* reset_storage(PyObject *self, PyObject *args)
{
	gpy_reset_storage();
	Py_RETURN_NONE;
}

static PyMethodDef gekv_module_methods[] = {
	{
		"init_gekv",
		init_gekv,
		METH_VARARGS,
		"Initializes Graph Engine Key-Value Store."
	},
	{
		"save_cell",
		save_cell,
		METH_VARARGS,
		"Saves the specified blob to as a cell."
	},
	{
		"update_cell",
		update_cell,
		METH_VARARGS,
		"Updates a saved cell using the specified blob."
	},
	{
		"remove_cell",
		remove_cell,
		METH_VARARGS,
		"Removes a cell from the key-value store."
	},
	{
		"load_cell",
		load_cell,
		METH_VARARGS,
		"Loads a cell with the specified cell id."
	},
	{
		"save_storage",
		save_storage,
		METH_NOARGS,
		"Dumps the in-memory key-value store to disk."
	},
	{
		"load_storage",
		load_storage,
		METH_NOARGS,
		"Loads the key-value store from disk to the main memory."
	},
	{
		"reset_storage",
		reset_storage,
		METH_NOARGS,
		"Resets in-memory key-value store to the initial state. The content in the memory storage will be cleared."
	},
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef gekv_module_definition = {
	PyModuleDef_HEAD_INIT,
	"gekv_module",
	"Graph Engine Key-Value store for Python.",
	-1,
	gekv_module_methods
};

PyMODINIT_FUNC PyInit_GEKV(void)
{
	Py_Initialize();
	return PyModule_Create(&gekv_module_definition);
}