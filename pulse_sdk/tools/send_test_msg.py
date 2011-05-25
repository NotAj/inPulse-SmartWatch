import bt_python.pulseprotocol as proto
from bt_python.bt_comm import BT_Socket
from optparse import OptionParser
import sys

usage = """%prog BD_ADDR -m "Message Text"
BD_ADDR in form 00:50:c2:xx:xx:xx
"""
parser = OptionParser(usage=usage)
parser.add_option('-m', '--message', 
					default='Hello world!',
					help='send a string like "HI GUYS!" to your watch. '
						 'Remember the quotes! Default is "Hello World"')
(options, args) = parser.parse_args()

if len(args) != 1:
	parser.error("incorrect number of arguments")
sock = BT_Socket(sys.argv[1], 0x1001)
print 'Sending message'
sock.send(proto.sms_notification('Test msg',options.message, 0))
print 'setting time on watch'
sock.send(proto.cmd_send_time())
sock.close()

