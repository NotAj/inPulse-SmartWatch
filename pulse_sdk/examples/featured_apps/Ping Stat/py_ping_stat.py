import tools.bt_python.pulseprotocol as proto
from tools.bt_python.bt_comm import BT_Socket
from optparse import OptionParser
import sys, os, re, time

usage = """%prog [options] bt_mac_address
bt_mac_address in form 00:50:c2:xx:xx:xx
"""
parser = OptionParser(usage=usage)
parser.add_option('-d', '--domain', 
					default='Example.com',
					help='Pick a domain to ping')
(options, args) = parser.parse_args()

if len(args) != 1:
	parser.error("incorrect number of arguments")

sock = BT_Socket(sys.argv[1], 0x1001)

# This script is designed to work on Linux and hasn't been tested
# on other operating system
# Ping command expected in format:
# 64 bytes from 208.68.143.50: icmp_req=1 ttl=114 time=45.7 ms
# It should work on a mac right away, but may require slight modification to run
# on Windows

while True:
	r = "".join(os.popen("ping options.domain -c 1").readlines())
	print r
	if re.search("64 bytes from", r):
	 results = r.split("time=")
	 result = results[1] 
	else:
	 result = "??? ms"
	print options.domain
	print result
	# Send this to the watch as an SMS notification
	# TODO: with options.domain, python seems to be sending over more than just
	# the domain. It doesn't affect the output on the watch though.
	sock.send(proto.sms_notification(result,options.domain, 0))
	# Set the time on the watch
	sock.send(proto.cmd_send_time())
	# Ping again in 60 seconds
	time.sleep(60)
sock.close()



