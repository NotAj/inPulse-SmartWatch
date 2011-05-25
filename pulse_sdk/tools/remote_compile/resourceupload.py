import os
from os.path import join
import random
import string
import sys
import mimetypes
import urllib2
import httplib
import time
import re
import zipfile

url = 'http://174.129.29.50:8080'
#thisfolder = os.getcwd() + '/' + sys.argv[1]

def random_string (length):
    return ''.join (random.choice (string.letters) for ii in range (length + 1))

def encode_multipart_data(files):
    boundary = random_string (30)

    def get_content_type(filename):
        return mimetypes.guess_type (filename)[0] or 'application/octet-stream'

    def encode_field(field_name):
        return ('--' + boundary,
                'Content-Disposition: form-data; name="%s"' % field_name,
                '', str (data [field_name]))

    def encode_file(field_name):
        filename = files[field_name]
        return ('--' + boundary,
                'Content-Disposition: form-data; name="%s"; filename="%s"' % (field_name, filename),
                'Content-Type: %s' % get_content_type(filename),
                '', open(filename, 'rb').read ())

    lines = []
    for name in files:
        lines.extend(encode_file (name))
    lines.extend(('--%s--' % boundary, ''))
    body = '\r\n'.join (lines)

    headers = {'content-type': 'multipart/form-data; boundary=' + boundary,
               'content-length': str (len (body))}

    return body, headers

def upload_file(url, path):
    files = {'usercode': path}
    req = urllib2.Request(url)
    connection = httplib.HTTPConnection(req.get_host ())
    connection.request('POST', req.get_selector (),
                        *encode_multipart_data (files))
    response = connection.getresponse ()
    return response.read()

def resourceupload( cwd, inputfile ):

	headerout = os.path.join(cwd,'include', 'app_resources.h')
	binout = os.path.join(cwd,'resources', 'inpulse_resources.bin')
	try:
		binaryurl = upload_file(url + '/resources', inputfile)
	except:
		print 'server is down, contact devsupport@getinpulse.com for more info'
		exit()

	filename = binaryurl.split('/')[-1] #grab end of url for filename

	time.sleep(.2)

	try:
		data = urllib2.urlopen(url + '/status/' + binaryurl)
	except:
		print "upload failed, contact devsupport@getinpulse.com"
		exit()
		
	print data.read(0)
	counter = 0
	while data.read() == "None" and counter < 30:
		data = urllib2.urlopen(url + '/status/' + binaryurl)
		print "not ready yet"
		counter += 1
		time.sleep(.5)
	else:
		if counter >20:
			print "resource server is broken. please contact devsupport@getinpulse.com"
			exit()
			
		log = urllib2.urlopen(url + '/static/' + binaryurl + '.log').read()
		print log
		try:	
			data1 = urllib2.urlopen(url + '/static/' + binaryurl + '.bin')
		except:
			print "resource building failed. See error report above."
			exit()
		if os.path.isfile(binout):
			os.remove( binout )
		fout = open( binout, 'wb') # creates the file where the uploaded file should be stored
		fout.write(data1.read()) # writes the uploaded file to the newly created file.
		fout.close() # closes the file, upload complete        
		print "Created resource pack: ", binout
		
		data2 = urllib2.urlopen(url + '/static/' + binaryurl + '.h')
		if os.path.isfile(headerout):
			os.remove( headerout )
		f2out = open( headerout, 'wb') # creates the file where the uploaded file should be stored
		f2out.write(data2.read()) # writes the uploaded file to the newly created file.
		f2out.close() # closes the file, upload complete        
		print "Created resource header: ", headerout

class ZipUtilities:

    def toZip(self, folder1, filename):
		zip_file = zipfile.ZipFile(filename, 'w')
		self.addFolderToZip(zip_file, folder1)
		zip_file.close()
		
    def addFolderToZip(self, zip_file, folder): 
        for file in os.listdir(folder):
            full_path = os.path.join(folder, file)
            if os.path.isfile(full_path):
                print 'File added: ' + str(full_path)
                zip_file.write(full_path)
            elif os.path.isdir(full_path):
                print 'Entering folder: ' + str(full_path)
                self.addFolderToZip(zip_file, full_path)
				
utilities = ZipUtilities()
print "Put all BMP, PNG and TTF files in resources/"
print "Be sure to update fonts.txt with your specific font selection"
if not os.path.isdir('resources'):
	print "Can't find directory 'resources/'"
	print "Make sure you run this script from the sdk root"
	exit()

utilities.toZip('resources','TEMP.zip')	
resourceupload( os.getcwd(), 'TEMP.zip' )
os.remove('TEMP.zip')
