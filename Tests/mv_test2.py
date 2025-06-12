from multiprocessing import Process, shared_memory, Pipe, Event

def sender(conn, done_event):
    data = bytearray(b"remote access test")
    shm = shared_memory.SharedMemory(create=True, size=len(data))
    shm.buf[:len(data)] = data
    print("[Sender] accessing its own object via memoryview")
    mvl = memoryview(shm.buf)
    print(mvl[0])

    conn.send((shm.name, len(data)))
    conn.close()

    done_event.wait()  # Wait for receiver to finish
    del mvl
    shm.close()
    shm.unlink()
    print("[Sender] Done. Exiting.")

def receiver(conn, done_event):
    shm_name, size = conn.recv()
    conn.close()

    shm = shared_memory.SharedMemory(name=shm_name)
    mvr = memoryview(shm.buf)[:size]
    print("[Receiver] accessing sender object via remote memoryview")
    print(mvr[1])

    del mvr
    shm.close()
    done_event.set()
    print("[Receiver] Done. Exiting.")

if __name__ == "__main__":
    parent_conn, child_conn = Pipe()
    done_event = Event()

    p1 = Process(target=sender, args=(parent_conn, done_event))
    p2 = Process(target=receiver, args=(child_conn, done_event))

    p1.start()
    p2.start()
    p1.join()
    p2.join()
