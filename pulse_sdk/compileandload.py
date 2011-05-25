from optparse import OptionParser
from tools.remote_compile.clientupload import ZipUtilities
from tools.remote_compile.clientupload import cloudcompile
from tools.bt_python.flashbin import flashbinary
import os
import zipfile
import time

usage = """%prog [options]
compiled binary will be stored in /build as pulse_app.bin
"""
parser = OptionParser(usage=usage)
parser.add_option("-d", dest="bdaddr",
                  help="Bluetooth BD Addr address in format 00:50:c2:xx:xx:xx")
parser.add_option("-c",
                  action="store_true", dest='compileonly', default=False,
                  help="compile only, don't load on inPulse")
parser.add_option("-r",
                  action="store_true", dest='loadresources', default=False,
                  help="load resources on inpulse, don't compile")
parser.add_option("-f", dest="resources_filename",
                  help="Resources file to send to inPulse", metavar="FILE")
parser.add_option("-l", dest="firmware_filename",
                  help="Firmware bin to send to inPulse", metavar="FILE")
parser.add_option("-n", action="store_true", dest="load_stock_notifications",
                  help="Download and load latest stock notifications app", metavar="FILE")


(options, args) = parser.parse_args()

# TODO: Clean up this messy option parsing logic
if not options.loadresources and not options.firmware_filename and not options.load_stock_notifications:
    utilities = ZipUtilities()
    print "all .c files must be in src/ and all .h files in include/"
    print "Use a flat directory structure, no directories in src/ or include/ at this moment"
    utilities.toZip('include','src','TEMP.zip')
    cloudcompile( os.getcwd(), 'TEMP.zip' )
elif options.loadresources:
    if options.resources_filename:
        resources_filename = options.resources_filename
    else:
        resources_filename = os.path.join(os.getcwd(), "resources", "inpulse_resources.bin")

if options.load_stock_notifications:
    firmware_url = "http://www.getinpulse.com/images/bbreleases/latest/pulseos.bin"
    resources_url = "http://www.getinpulse.com/images/bbreleases/latest/resources.bin"
    flashbinary( options.bdaddr, firmware_url, 0x8000, True, True)
    print "Rebooting watch ..."
    time.sleep(20)
    flashbinary( options.bdaddr, resources_url, 0x40000, True, True)
elif not options.compileonly and not options.loadresources:
    print "About to load firmware image"
    if options.firmware_filename:
        flashbinary( options.bdaddr, options.firmware_filename, 0x8000, False, True)
    else:
        flashbinary( options.bdaddr, os.path.join(os.getcwd(), "build", "pulse_app.bin" ), 0x8000, False, True)
    os.remove('TEMP.zip')
elif options.loadresources:
    print "About to load resources:"
    print resources_filename
    flashbinary( options.bdaddr, resources_filename, 0x40000, False, True)
elif options.compileonly:
    print "Load code on inPulse: 'python ./compileandload.py -d 00:50:c2:xx:xx:xx -l " + os.path.join('build', 'pulse_app.bin') + "'"
    os.remove('TEMP.zip')
