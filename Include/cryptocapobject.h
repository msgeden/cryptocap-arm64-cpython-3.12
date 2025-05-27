#ifndef Py_CRYPTCAPOBJECT_H
#define Py_CRYPTCAPOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Python.h"

/* Definition of the CryptoCap object */

typedef struct {
    PyObject_HEAD
    uint64_t perms_base;  // packed perms and base address
    uint32_t offset;
    uint32_t size;        // in bytes
    uint64_t PT;          // Placeholder (e.g., page table reference)
    uint64_t MAC;         // Placeholder (e.g., integrity check)
} PyCryptoCapObject;

/* Definition of the RemoteArray object */

typedef struct {
    PyObject_HEAD
    PyCryptoCapObject* cap;  // Associated capability
    size_t elem_size;        // e.g., 1 for byte-level
} RemoteArrayObject;

#ifdef __cplusplus
}
#endif
#endif /* !Py_CRYPTCAPOBJECT_H */
