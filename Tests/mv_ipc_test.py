import struct
import sys
import ctypes

from multiprocessing import Pipe, Event, Process

def sender(conn, done_event):

    data = bytearray(b"remote access test")
    mv = memoryview(data)
    #mv = memoryview(data, remote=True)
    conn.send(mv)  
    done_event.wait()
    print("[Sender] Done. Exiting.")

def receiver(conn, done_event):
    
    mv = conn.recv()  # receive cryptocap object
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
