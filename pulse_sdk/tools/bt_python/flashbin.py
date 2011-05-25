import sys
import time, os
import pulseprotocol as proto
from bt_comm import BT_Socket
import urllib
import time

try:
	btmac = sys.argv[1]
	binfile = sys.argv[2]
	addr = sys.argv[3]
except:
	print 'use correct syntax'

def handle_error(btmac, socket):
    socket.close()
    socket = BT_Socket(btmac, 0x1001)
    return socket

def flashbinary( btmac, binfile, addr, is_url, reboot):
	tries = 1
	while True:
		try:			
			socket = BT_Socket(btmac, 0x1001)
			break
		except:
			tries += 1
		if tries > 3:
			print "invalid, missing, or unavailable bd addr"
			sys.exit()

	proto.determine_packet_format(socket)
	socket.send(proto.cmd_set_bt_sniff_off())

	if (is_url):
		urllib.urlretrieve(binfile, "temp.bin")
		f = open("temp.bin", 'rb')
	else:
		f = open(binfile, 'rb')
	fileLen = len(f.read())

	numSectorsToErase = fileLen / 4096 + (fileLen % 4096 > 0)
	print "Preparing ",
        if proto.get_in_vlf() and addr == 0x40000:
            # TODO: I don't like having the hard-coded addr check here
            proto.cmd_erase_resources(fileLen)
        else:
            for i in range(numSectorsToErase):
                    socket.send(proto.cmd_erase_spiflash_sector((addr / 4096) + i))
                    resp_len = len(socket.receive(1024))
                    # TODO FIX code duplication here
                    if resp_len >= 4:
                        error_count = 0
                        print ".",
                        #time.sleep(0.5)
                    else:
                        print "Retrying packet ..."
                        print resp_len
                        error_count = error_count + 1
                        if error_count == 5:
                            socket = handle_error(btmac, socket)
                            error_count = 0
                    sys.stdout.flush()
            print ""

	f.seek(0)
	bytesToSend = fileLen
	addr_offset = 0
	if proto.get_in_vlf():
		data_packet_chunk = 128
	else:
		data_packet_chunk = 220

	data = f.read(data_packet_chunk)
        error_count = 0
	while (len(data) != 0):
		print bytesToSend
		socket.send(proto.write_spiflash(addr + addr_offset, data))
                resp_len = len(socket.receive(1024))
                # TODO FIX code duplication here
		if resp_len >= 4:
                    error_count = 0
                    addr_offset += data_packet_chunk
                    f.seek(addr_offset)
                    data = f.read(data_packet_chunk)
                    bytesToSend -= data_packet_chunk
                    #time.sleep(0.5)
                else:
                    print "Retrying packet ..."
                    print resp_len
                    error_count = error_count + 1
                    if error_count == 5:
                        socket = handle_error(btmac, socket)
                        error_count = 0

	socket.send(proto.cmd_send_time())

	if is_url:
		f.close()
		os.remove("temp.bin")

	if reboot:
		socket.send(proto.cmd_reboot_watch())
	socket.close()
