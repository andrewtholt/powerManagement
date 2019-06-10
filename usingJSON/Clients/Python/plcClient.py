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
            self.sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
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
        return (data.decode('utf-8')).strip()

    def set(self,name, value):
        cmd = "SET " + name + " " + value + "\n"
        self.mysend( cmd )
        
    def get(self,name):
        cmd = "GET " + name + "\n"
        self.mysend( cmd )

        return( self.myreceive())

    def getBoolean(self,name):
        val = False
        res = self.get(name)

        if res in ['ON','TRUE']:
            val = True

        return val

    def setBoolean(self, name, value):
        v = "FALSE"

        if value:
            v = "ON"
        else:
            v="OFF"

        self.set(name, v)


if __name__ == "__main__" :
    tst = MySocket()

    tst.connect("127.0.0.1", 9191)

    tst.setBoolean("TEST",True)

    time.sleep(1)

    print( tst.getBoolean("TEST"))

