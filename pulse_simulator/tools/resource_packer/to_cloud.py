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
#url = 'http://ec2-50-16-103-63.compute-1.amazonaws.com:8080'
#thisfile = os.getcwd() + '/' + sys.argv[1]

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

def cloudcompile( cwd, inputfile ):

	outputfile = os.path.join(cwd,'build', 'pulse_app.bin')
	hexout = os.path.join(cwd,'build', 'pulse_app.hex')
	
	try:
		binaryurl = upload_file(url + '/upload', inputfile)
	except:
		print 'server is down, contact eric@allerta.ca for more info'
		exit()

	filename = binaryurl.split('/')[-1] #grab end of url for filename

	time.sleep(1)

	data = urllib2.urlopen(url + '/status/' + binaryurl)
	print data.read(0)
	counter = 0
	while data.read() == "None" and counter < 30:
		data = urllib2.urlopen(url + '/status/' + binaryurl)
		print "not ready yet"
		counter += 1
		time.sleep(.5)
	else:
		if counter >20:
			print "compile server is broken. please contact support@allerta.ca"
			exit()
			
		log = urllib2.urlopen(url + '/static/' + binaryurl + '.log').read()
		print log
		try:
			data = urllib2.urlopen(url + '/static/' + binaryurl + '.bin')
		except:
			print "Compile failed. See error report above"
			exit()
		if os.path.isfile(outputfile):
			os.remove( outputfile )
		fout = open( outputfile, 'wb') # creates the file where the uploaded file should be stored
		fout.write(data.read()) # writes the uploaded file to the newly created file.
		fout.close() # closes the file, upload complete        
		print "Compiled binary: ", outputfile
		
		hexdata = urllib2.urlopen(url + '/static/' + binaryurl + '.hex')
		if os.path.isfile(hexout):
			os.remove( hexout )
		fout2 = open( hexout, 'wb') # creates the file where the uploaded file should be stored
		fout2.write(hexdata.read()) # writes the uploaded file to the newly created file.
		fout2.close() # closes the file, upload complete        
		print "Compiled binary: ", hexout

class ZipUtilities:

    def toZip(self, folder1, folder2, filename):
		zip_file = zipfile.ZipFile(filename, 'w')
		self.addFolderToZip(zip_file, folder1)
		self.addFolderToZip(zip_file, folder2)
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
