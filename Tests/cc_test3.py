import struct
import cc_module
import sys
import ctypes
import numpy as np

from multiprocessing import Pipe, Event, Process

def sender(conn, done_event):

	arr = np.frombuffer(b"HELLO WORLD", dtype=np.uint8)
	
	cap = cc_module.create_cap(arr.ctypes.data, 0, arr.nbytes, True)
	print(f"[Sender] Created cap: {cap}")    

	conn.send(cap)  
	done_event.wait()
	print("[Sender] Done. Exiting.")

def receiver(conn, done_event):
    
	cap = conn.recv()  # receive cryptocap object
	print(f"[Receiver] Received cap: {cap}")    
	rarr = cc_module.CCArray(cap)
	print("[Receiver] Accessing remote chars:")
	
	for i in range(len(rarr)):
		print(f"  rarr[{i}] = '{chr(rarr[i])}'")
 
	conn.close()
	done_event.set()

if __name__ == "__main__":

	parent_conn, child_conn = Pipe()
	done_event = Event()

	p1 = Process(target=sender, args=(parent_conn, done_event))
	p2 = Process(target=receiver, args=(child_conn, done_event))

	p1.start()
	p2.start()
	p1.join()	
	p2.join()
