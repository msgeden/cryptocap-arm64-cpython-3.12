#ifndef Py_CRYPTOCAP_H
#define Py_CRYPTOCAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Python.h"

void cc_isa_nop(int arg1, int arg2);
void cc_isa_store_cap_from_CR0(cc_dcap* cap) ;
cc_dcap cc_isa_create_cap(const void* base, size_t offset, size_t size, int write_flag);
void cc_isa_load_ver_cap_to_CR0(cc_dcap* cap);
uint8_t* cc_isa_load_CR0_memcpy(void* dst, cc_dcap src, size_t count);
uint8_t cc_isa_read_i8_via_CR0();
void cc_isa_write_i8_via_CR0(uint8_t data);
uint8_t cc_isa_load_CR0_read_i8_data(cc_dcap cap);
void cc_isa_load_CR0_write_i8_data(cc_dcap cap, uint8_t data);

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_CRYPTOCAP_H */