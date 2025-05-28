import cc_module
import sys
import ctypes



# Allocate a mutable byte
buf = (ctypes.c_uint8 * 2)()
buf[0] = 63
addr = ctypes.addressof(buf)

cap = cc_module.create_cap(addr, 0, 5, True)
print(f"created capability: {cap}")

print("original value:", buf[0])
val = cc_module.read_i8_via_CR0()
print("pre-write value:", val)
cc_module.write_i8_via_CR0(66)
print("post-write value:", buf[0])  # should reflect the write

arr = cc_module.CCArray(cap)
print("arr-length:", len(arr))
print("arr-first byte:", arr[0])
