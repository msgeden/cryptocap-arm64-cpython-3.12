#ifndef CC_MODULE_H
#define CC_MODULE_H

#include <stdio.h>
#include <stdlib.h>
#include "pycryptocap.h"

typedef struct {
    PyObject_HEAD
    cc_dcap cap;
    size_t elem_size;  // 1 for bytes
} CCArrayObject;

#endif