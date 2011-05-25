import sys
import time, os
import pulseprotocol as proto
from bt_comm import BT_Socket

try:
	btmac = sys.argv[1]
	binfile = sys.argv[2]
except:
	print 'user correct syntax'
def flashbinary( btmac, binfile):
	try:
		socket = BT_Socket(btmac, 0x1001)
	except:
		print "bad mac address"
		sys.exit()

	proto.determine_packet_format(socket)
	socket.send(proto.cmd_set_bt_sniff_off())

	f = open(binfile, 'rb')
	fileLen = len(f.read())

	numSectorsToErase = fileLen / 4096 + (fileLen % 4096 > 0)

	for i in range(numSectorsToErase):
		socket.send(proto.cmd_erase_spiflash_sector(8 + i))
		byte = socket.receive(4)

	f.seek(0)
	bytesToSend = fileLen
	addr_offset = 0
	data = f.read(220)

	while (len(data) != 0):
		print bytesToSend
		socket.send(proto.write_spiflash(32768 + addr_offset, data))
		socket.receive(4)
		addr_offset += 220
		f.seek(addr_offset)
		data = f.read(220)
		bytesToSend -= 220

	socket.send(proto.cmd_send_time())
	socket.send(proto.cmd_reboot_watch())
	socket.close()