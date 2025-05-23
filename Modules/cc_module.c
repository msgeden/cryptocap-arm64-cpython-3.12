#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cc_module.h"

// Python-accessible function
static PyObject* py_execute_cc_nop(PyObject* self, PyObject* args) {
    int arg1, arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2)) {
        return NULL;  // TypeError if args are invalid
    }
    execute_cc_nop(arg1, arg2);  // Call inline assembly
    Py_RETURN_NONE;  // Return None to Python
}


// Python-accessible function
static PyObject* py_execute_cc_print_cap(PyObject* self, PyObject* args) {
    cc_dcap cap;
    if (!PyArg_ParseTuple(args, "ii", &cap)) {
        return NULL;  // TypeError if args are invalid
    }
    execute_cc_print_cap(cap);  // Call inline assembly
    Py_RETURN_NONE;  // Return None to Python
}


// Python-accessible function
static PyObject* py_execute_cc_create_cap(PyObject* self, PyObject* args) {
    unsigned long base_addr;
    unsigned int offset;
    unsigned int size;
    int write_flag;
    
    // Parse Python args: (base_address, offset, size, write_flag)
    if (!PyArg_ParseTuple(args, "kIIp", &base_addr, &offset, &size, &write_flag)) {
        return NULL;  // TypeError if args are invalid
    }
    
    // Create capability
    cc_dcap cap = execute_cc_create_cap((void*)base_addr, offset, size, (bool)write_flag);
    
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
static PyObject* py_execute_cc_memcpy(PyObject* self, PyObject* args) {
    void* dst;
    cc_dcap src;
    size_t count;
    if (!PyArg_ParseTuple(args, "ii", &dst, &src, &count)) {
        return NULL;  // TypeError if args are invalid
    }
    execute_cc_memcpy(dst, src, count);  // Call inline assembly
    Py_RETURN_NONE;  // Return None to Python
}



// Method definition
static PyMethodDef CustomMethods[] = {
    {"nop", py_execute_cc_nop, METH_VARARGS, "Execute custom ISA instruction."},
    {"print_cap", py_execute_cc_print_cap, METH_VARARGS, "Print cap info."},
    {"create_cap", py_execute_cc_create_cap, METH_VARARGS, "Execute ccreate ISA instruction."},
    {"memcpy", py_execute_cc_memcpy, METH_VARARGS, "Execute memcpy ISA logic."},

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
PyMODINIT_FUNC PyInit_cc_module(void) {
    return PyModule_Create(&custommodule);
}