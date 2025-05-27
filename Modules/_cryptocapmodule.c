#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../Include/cryptocapobject.h"  // Adjust path as needed

// Minimal implementation for CryptoCap type
static PyObject *
CryptoCap_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyCryptoCapObject *self;
    self = (PyCryptoCapObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->perms_base = 0;
        self->offset = 0;
        self->size = 0;
        self->PT = 0;
        self->MAC = 0;
    }
    return (PyObject *)self;
}

static int
CryptoCap_init(PyCryptoCapObject *self, PyObject *args, PyObject *kwds) {
    return PyArg_ParseTuple(args, "KIIKK", &self->perms_base, &self->offset,
                            &self->size, &self->PT, &self->MAC) ? 0 : -1;
}

static PyTypeObject PyCryptoCap_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_cryptocap.CryptoCap",
    .tp_basicsize = sizeof(PyCryptoCapObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CryptoCap_new,
    .tp_init = (initproc)CryptoCap_init,
    .tp_doc = "CryptoCap(perms_base, offset, size, PT, MAC)",
};

// Minimal implementation for RemoteArray type
static PyObject *
RemoteArray_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    RemoteArrayObject *self;
    PyObject *cap_arg;

    if (!PyArg_ParseTuple(args, "O!", &PyCryptoCap_Type, &cap_arg)) {
        return NULL;
    }

    self = (RemoteArrayObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        Py_INCREF(cap_arg);
        self->cap = (PyCryptoCapObject *)cap_arg;
        self->elem_size = 1;  // mock
    }
    return (PyObject *)self;
}

static Py_ssize_t
RemoteArray_len(RemoteArrayObject *self) {
    return self->cap->size;
}

static PyObject *
RemoteArray_getitem(RemoteArrayObject *self, Py_ssize_t index) {
    if (index < 0 || index >= self->cap->size) {
        PyErr_SetString(PyExc_IndexError, "Out of bounds (capability-limited)");
        return NULL;
    }
    char val = 'A' + (char)(index % 26);  // mock
    return PyUnicode_FromStringAndSize(&val, 1);
}

static PySequenceMethods RemoteArray_as_sequence = {
    .sq_length = (lenfunc)RemoteArray_len,
    .sq_item = (ssizeargfunc)RemoteArray_getitem,
};

static PyTypeObject RemoteArray_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_cryptocap.RemoteArray",
    .tp_basicsize = sizeof(RemoteArrayObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = RemoteArray_new,
    .tp_as_sequence = &RemoteArray_as_sequence,
    .tp_doc = "RemoteArray(CryptoCap)",
};

// Module definition
static struct PyModuleDef cryptocapmodule = {
    PyModuleDef_HEAD_INIT,
    "_cryptocap",
    "Capability-based remote memory module",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit__cryptocap(void) {
    if (PyType_Ready(&PyCryptoCap_Type) < 0)
        return NULL;
    if (PyType_Ready(&RemoteArray_Type) < 0)
        return NULL;

    PyObject *m = PyModule_Create(&cryptocapmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyCryptoCap_Type);
    Py_INCREF(&RemoteArray_Type);
    PyModule_AddObject(m, "CryptoCap", (PyObject *)&PyCryptoCap_Type);
    PyModule_AddObject(m, "RemoteArray", (PyObject *)&RemoteArray_Type);
    fprintf(stderr, "[INFO] Initializing _cryptocap module...\n");
    return m;
}
