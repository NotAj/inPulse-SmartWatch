import bt_python.pulseprotocol as proto
from bt_python.bt_comm import BT_Socket
from optparse import OptionParser
import sys


print 'Usage: python set_time.py BD_ADDR'
print 'Where BT_ADDR in form 00:50:c2:xx:xx:xx'


sock = BT_Socket(sys.argv[1], 0x1001)
print 'Setting current time on your watch'
sock.send(proto.cmd_send_time())
sock.close()
