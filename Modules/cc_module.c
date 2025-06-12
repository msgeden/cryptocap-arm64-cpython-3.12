#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cc_module.h"

// Python-accessible function
static PyObject* py_cc_nop(PyObject* self, PyObject* args) {
    int arg1, arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2)) {
        return NULL;  // TypeError if args are invalid
    }
    cc_isa_nop(arg1, arg2);  // Call inline assembly
    Py_RETURN_NONE;  // Return None to Python
}

// Python-accessible function
static PyObject* py_cc_create_cap(PyObject* self, PyObject* args) {
    unsigned long base_addr;
    unsigned int offset;
    unsigned int size;
    int write_flag;
    
    // Parse Python args: (base_address, offset, size, write_flag)
    if (!PyArg_ParseTuple(args, "kIIp", &base_addr, &offset, &size, &write_flag)) {
        return NULL;  // TypeError if args are invalid
    }
    
    // Create capability
    cc_dcap cap = cc_isa_create_cap((void*)base_addr, offset, size, write_flag);
    
    // cc_dcap cap;
    // cap.perms_base = (write_flag ?  WRITE + READ : READ);  // Example: READ + WRITE if write_flag is true
    // cap.perms_base = (cap.perms_base << 48) | (uint64_t)base_addr;  // Pack perms and base address
    // cap.offset = offset;
    // cap.size = size;  // Size in bytes
    // cap.PT = 0;  // Placeholder for page table reference
    // cap.MAC = 0;  // Placeholder for integrity check

    // Return as Python dictionary
    PyObject* cap_dict = Py_BuildValue(
        "{s:K, s:I, s:I, s:K, s:K}",
        "perms_base", cap.perms_base,
        "offset", cap.offset,
        "size", cap.size,
        "PT", cap.PT,
        "MAC", cap.MAC
    );
    
    return cap_dict;
}


// Python-accessible function
static PyObject* py_read_i8_data_via_CR0(PyObject* self, PyObject* args) {
    uint8_t ret=cc_isa_read_i8_via_CR0();
    //uint8_t ret='A';  // Initialize ret with a mock value
    return PyLong_FromUnsignedLong((unsigned long)ret);  // Return the read value as a Python integer
}

// Python-accessible function
static PyObject* py_write_i8_data_via_CR0(PyObject* self, PyObject* args) {
    uint8_t data;
    if (!PyArg_ParseTuple(args, "B", &data)) {
        return NULL;  // TypeError if args are invalid
    }
    cc_isa_write_i8_via_CR0(data);  // Call inline assembly to write data
    Py_RETURN_NONE;
}

// Python-accessible function
static PyObject* py_cc_memcpy(PyObject* self, PyObject* args) {
    void* dst;
    cc_dcap src;
    size_t count;
    if (!PyArg_ParseTuple(args, "ii", &dst, &src, &count)) {
        return NULL;  // TypeError if args are invalid
    }
    cc_isa_load_CR0_memcpy(dst, src, count);  // Call inline assembly
    Py_RETURN_NONE;  // Return None to Python
}



// Method definition
static PyMethodDef CustomMethods[] = {
    {"nop", py_cc_nop, METH_VARARGS, "Execute a nop instruction."},
    {"create_cap", py_cc_create_cap, METH_VARARGS, "Execute ccreate ISA instruction."},
    {"read_i8_via_CR0", py_read_i8_data_via_CR0, METH_VARARGS, "Execute cldg8 ISA instruction."},
    {"write_i8_via_CR0", py_write_i8_data_via_CR0, METH_VARARGS, "Execute cstg8 ISA instruction."},
    {"memcpy", py_cc_memcpy, METH_VARARGS, "Execute memcpy ISA logic."},
    {NULL, NULL, 0, NULL}  // Sentinel
};


// Module definition
static struct PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    "cc_module",  // Module name
    NULL,                 // Module docstring
    -1,                   // Global state
    CustomMethods         // Method table
};

// Module initialization
// PyMODINIT_FUNC PyInit_cc_module(void) {
//     return PyModule_Create(&custommodule);
// }

static PyObject *CCArray_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyObject *dict;
    if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
        return NULL;
    }

    CCArrayObject *self = (CCArrayObject *)type->tp_alloc(type, 0);
    if (!self) return NULL;

    self->cap.perms_base = PyLong_AsUnsignedLongLong(PyDict_GetItemString(dict, "perms_base"));
    self->cap.offset     = PyLong_AsUnsignedLong(PyDict_GetItemString(dict, "offset"));
    self->cap.size       = PyLong_AsUnsignedLong(PyDict_GetItemString(dict, "size"));
    self->cap.PT         = PyLong_AsUnsignedLongLong(PyDict_GetItemString(dict, "PT"));
    self->cap.MAC        = PyLong_AsUnsignedLongLong(PyDict_GetItemString(dict, "MAC"));
    self->elem_size = 1;

    return (PyObject *)self;
}

static Py_ssize_t
CCArray_len(CCArrayObject *self) {
    return self->cap.size;
}

static PyObject *
CCArray_getitem(CCArrayObject *self, Py_ssize_t index) {
    if (index < 0 || index >= self->cap.size) {
        PyErr_SetString(PyExc_IndexError, "Out of bounds");
        return NULL;
    }

    uint8_t result;
    cc_dcap temp = self->cap;
    temp.offset += index;

    //result = 'A';  // Initialize result
    result=cc_isa_load_CR0_read_i8_data(temp);
    return PyLong_FromUnsignedLong((unsigned long)result);
}

static PySequenceMethods CCArray_seqmethods = {
    .sq_length = (lenfunc)CCArray_len,
    .sq_item   = (ssizeargfunc)CCArray_getitem,
};

static PyTypeObject CCArray_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cc_module.CCArray",
    .tp_basicsize = sizeof(CCArrayObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CCArray_new,
    .tp_as_sequence = &CCArray_seqmethods,
    .tp_doc = "CCArray(cap_dict)",
};


PyMODINIT_FUNC PyInit_cc_module(void) {
    if (PyType_Ready(&CCArray_Type) < 0)
        return NULL;

    PyObject *m = PyModule_Create(&custommodule);
    if (!m)
        return NULL;

    Py_INCREF(&CCArray_Type);
    PyModule_AddObject(m, "CCArray", (PyObject *)&CCArray_Type);
    return m;
}
