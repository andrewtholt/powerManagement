import socket

import sys

import time

from io import StringIO

class MySocket:
    """demonstration class only
      - coded for clarity, not efficiency
    """

    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                            socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def mysend(self, m):
        totalsent = 0
        msg=""
        msg=bytes(m,'utf-8')
        print( msg )
        self.sock.sendall(msg)

        return

        while totalsent < len(m) :
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

    def myreceive(self):
        buff = StringIO()
        chunks = []
        bytes_recd = 0

        while True:
            data = self.sock.recv(10)
            msg=data.decode('utf-8')
            buff.write(msg)

            if b'\n' in data:
                break
        return data.decode('utf-8')

#        while bytes_recd < MSGLEN:
#           chunk = self.sock.recv(min(MSGLEN - bytes_recd, 2048))
#            if chunk == b'':
#                raise RuntimeError("socket connection broken")
#            chunks.append(chunk)
#            bytes_recd = bytes_recd + len(chunk)
#        return b''.join(chunks)


if __name__ == "__main__" :
    tst = MySocket()

    tst.connect("127.0.0.1", 9191)

    tst.mysend("SET TEST OK\n")

    time.sleep(1)

    print( tst.myreceive())

