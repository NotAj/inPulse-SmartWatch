#!/usr/bin/env python

"""
Firmware Checksummer

USAGAE: %s <hexfile> <output>

This script takes a firmware hexfile and inserts a checksum word at
0x1000 in the firmware. It outputs the checksummed firmware as a flat
binary file, suitable for loading via SPISP or over bluetooth.

The checksum is a CRC32 (PNG/ZIP checksum) of all bytes of firmware
from [0x1004, 0x8000], padding the firmware with 0xFF if necessary.
The checksum is stored in little-endian format at 0x1003-0x1000.

The outputted file will always be exactly 32768 bytes long.

"""

import binascii, sys, struct
from intelhex import IntelHex

if len(sys.argv) != 3:
    print __doc__ % sys.argv[0]
    sys.exit(1)

ih = IntelHex(sys.argv[1])
out = open(sys.argv[2], 'wb')

# Sanity checks, very important
if ih.maxaddr() < 0x1004 or ih.maxaddr() > 32767:
    print "Insane hexfile: min=0x%x max=0x%x" % (ih.minaddr(), ih.maxaddr())
    sys.exit(2)
print "Length: %d / 28672" % (ih.maxaddr() - 4096 + 1)
print "Free: %d" % (28672 - (ih.maxaddr() - 4096 + 1))

# Hack to force tobinstr() to write data from addres 0. IntelHex will 
# only write data from the first location with initialized data, skipping
# over any uninitialized data beforehand. This initializes address 0,
# forcing IntelHex to do what I want.
ih[0]=ih[0]

sumdata = (ih.tobinstr())[0x1004:]
sumdata += '\xff' * (0x6ffc - len(sumdata))
cksum = binascii.crc32(sumdata) & 0xffffffff
print "Checksum: 0x%08x" % cksum
ih.puts(0x1000, struct.pack('<L', cksum))

# Rather than using IntelHex.tobinfile(), i'm writing to a file this way
# to ensure that the file ends up at exactly 32768 bytes.
binfw = ih.tobinstr()
binfw += '\xff' * (0x8000 - len(binfw))
out.write(binfw)
out.close()

# Testing only.
a=IntelHex()
a.loadbin(sys.argv[2])
a.write_hex_file(sys.argv[2] + '.hex')
