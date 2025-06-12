import struct
import cc_module
import sys
import ctypes

arr = b"hello"
cap1 = cc_module.create_cap(arr, id(arr), 0, len(arr), True)
print(cap1["type_name"])  # Should print 'bytes' or equivalent
ch = 'a'
cap2 = cc_module.create_cap(ch, id(ch), 0, 1, True)
print(cap2["type_name"])  # Should print 'bytes' or equivalent

