import struct
import cc_module
import sys
import ctypes
from ctypes import create_string_buffer, addressof

var = 42
cap_int = cc_module.create_cap(var, id(var), 0, 4, False)  # Will store type_name = "int"
print(f"created cap_int: {cap_int}")    
obj_int = cc_module.read_via_cap(cap_int)
print(obj_int + 10)  # Should print 52

# s = create_string_buffer(b"hello world")
# print(s.value.decode())  # b'hello world\x00'
# addr = addressof(s)
# cap_str = cc_module.create_cap(s, addr, 0, len(s.value), True)
# cap_str["type_name"] = "str"  # <-- Fix here
# print(f"created cap_str: {cap_str}")    

# # Receiver side: read it back using the cap
# restored = cc_module.read_via_cap(cap_str)
# print(restored)
# print(f"Message received: {restored}")