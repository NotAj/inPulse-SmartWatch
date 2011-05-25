import sys
import win32com.client #used to control iTunes
import pulseprotocol as proto
from bt_comm import BT_Socket

#Establishing connection with the watch
btmac = sys.argv[1]
try:
	socket = BT_Socket(btmac, 0x1001)
except:
	print "Bad mac address, or if on Windows, the L2CAPServer might not be wokring"
	sys.exit()
proto.determine_packet_format(socket)
socket.send(proto.cmd_set_bt_sniff_off())
socket.receive(4)

#Establishing connection with iTunes
iTunes = win32com.client.gencache.EnsureDispatch("iTunes.Application")
iTunes.Pause();

while(1):
	#Receive the commain from the watch
	text=socket.receive(256)
	#If it's a valid command (Note: non-valid commands will have a length of 2)
	if(len(text) is 4):
		#transform it into an int	
		key=proto.int_from_buffer(text)
		if(key is 1):
			iTunes.PlayPause()
		elif (key is 0):
			iTunes.NextTrack()
		#Sends back the artist name and track number
		socket.send(iTunes.CurrentTrack.Artist)
		socket.send(iTunes.CurrentTrack.Name)
#Closes conncetions	
socket.close()
