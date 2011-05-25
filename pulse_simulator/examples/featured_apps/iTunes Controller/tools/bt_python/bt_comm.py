import sys, atexit

if sys.platform != 'win32':
    try:
        import lightblue
    except:
        print "please install lightblue: "
        if sys.platform == 'darwin':
            print "from dpkg: http://getinpulse.com/hack/downloads/lightblue.dpkg"
        else:
            print "On Ubuntu..."
            print "sudo apt-get install python-lightblue"
            print "sudo apt-get install python-bluez"
        sys.exit(1)
else:
	import socket

class BT_Socket:
    def __init__(self, bdaddr, port=0x1001):
        atexit.register(self.__del__)
        if sys.platform == 'win32':
            # Connect to L2Cap server if running on windows
            self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            addr = "localhost"
            port = 1115
        else:
            self._sock = lightblue.socket( lightblue.L2CAP )
            addr = bdaddr
        print "trying to connect to"
        self._sock.connect((addr, port))
        if sys.platform == 'win32':
			# Send L2Cap Socket Server a connect message
            self._sock.send('\x20\x00\x00\x00')
            self._sock.send(bdaddr.replace(":",""))
            # wait for an ACK
            buf = self._sock.recv(4)
            
    def send(self, message):
        if sys.platform == 'win32':
            # sending data
            self._sock.send('\x00\x00\x00\x00')
            self._sock.send(message)
            
            # wait for an ACK
            buf = self._sock.recv(4)
            print "Blah"
            print len(buf)
        else:
            self._sock.send(message)
            
    def receive(self, buffersize):
        if sys.platform == 'win32':
            self._sock.send('\x01\x00\x00\x00')
            self._sock.send('\x00\x00\x00\x00')
            print "Before receiving"
            buf = self._sock.recv(4)
            print "After receiving"
            print len(buf)
            return buf
        else:
            return self._sock.recv(buffersize)
        
    def close(self):
        if sys.platform == 'win32':
            self._sock.send('\x30\x00\x00\x00')
            self._sock.send(" ")
        return self._sock.close()

    def __del__(self):
        try:
            self._sock.close()
        except: pass
