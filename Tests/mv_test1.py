data = bytearray(b"remote access test")
mvl = memoryview(data)
mvr = memoryview(data, remote=True)
print(mvl[0])  
print(mvr[1])  # Should trigger your remote logic
