#!/usr/bin/env python

# The FTFS image will be created in the same folder from which this script
# is run.

import sys
import os
import struct
import crc32
import gzip
import tempfile
import shutil
import glob

if len(sys.argv) < 4:
	print("usage: %s <backend_api_version> <image> <sourcedir> <recipe_file> <web_repo>" % sys.argv[0])
	sys.exit(0)


MAGIC = "\x88\x88\x88\x88\x63\x6F\x7A\x79"
MAX_NAME_LEN = 23
VERSION = "0100"
VERBOSE=0

TMPDIR=tempfile.mkdtemp()

FTFS_IMG=os.path.abspath(sys.argv[2])

CURDIR=os.getcwd()

# List of file extensions to be compressed.
TO_GZIP_FILES= [""]

def pack_files(table):
	summary = ""
	buffer = ""	

	offset = (len(table)+1)*32 # each table entry is 32 bytes long

	for i in range(0, len(table)):
		h = open(table[i][0], "rb")
		data = h.read()
		h.close()
		if i > 0:
			table[i][2] += table[i-1][2]+table[i-1][3]
		else:
			table[i][2] = offset
		table[i][3] = len(data)
		buffer += data
		summary += table[i][1] + \
				'\0'*(MAX_NAME_LEN+1-len(table[i][1])) + \
				struct.pack('ii', table[i][2], table[i][3])

	summary += '\0'*32 # add terminating entry

	pad = len(summary+buffer)%4
	if pad > 0:
		buffer += chr(0xFF)*(4-pad)

	return summary+buffer

def compress_given_files(dir):
	try:
		path_list = os.listdir(dir)
	except OSError:
		print("error: cannot open source path \"%s\"" % dir)
		sys.exit(1)
	for path in path_list:
		if os.path.isdir(path):
			continue
		if len(path) >= MAX_NAME_LEN:
			print("error: file name \"%s\" is too long" % path)
			return None
		fileWOext, fileExtension = os.path.splitext(path)
		for givenExt in TO_GZIP_FILES:
			if fileExtension == givenExt and fileExtension != "":
				gzPath = path + ".gz"
				f_in = open(path, 'rb')
				f_out = gzip.open(gzPath, 'wb')
				f_out.writelines(f_in)
				f_out.close()
				f_in.close()
				if VERBOSE == 1:
					normal_size = size = os.path.getsize(path)
					compressed_size = size = os.path.getsize(gzPath)
					print("Compressed \"%s\" Size reduction %d%%" \
					% (path, \
					   ((normal_size - compressed_size) * 100)/normal_size))
				os.remove(path);


def init_file_table(dir, paths):
	file_table = []

	for path in paths:
		if os.path.isdir(path):
			continue
		if len(path) >= MAX_NAME_LEN:
			print("error: file name \"%s\" is too long" % path)
			return None
		file_table.append([dir+path,path,0,0])

	return file_table

def copy_files_from_dir(dest_dir, src_path):
	for fname in glob.iglob(src_path):
		if os.path.isfile(fname):
			shutil.copy(fname,dest_dir)
		else:
			copy_files_from_dir(dest_dir, fname + "/*")


def copy_files_from_recipe(dest_dir, commondir_path , recipe_file):
	fd = open(recipe_file)
	for line in fd:
		copy_files_from_dir(dest_dir, commondir_path+line.strip())
	return None

if __name__ == '__main__':

	# Copy the source tree
	WORK_DIR=TMPDIR + "/ftfs-img/"
	if VERBOSE == 1:
		print("Temp. dir %s" % TMPDIR)

	if not os.path.exists(TMPDIR):
		print("error: cannot open source path \"%s\"" % TMPDIR)
		sys.exit(1)

	shutil.copytree(sys.argv[3], WORK_DIR)

	recipe_file = None
	# Check if user want to copy files from recipe
	if len (sys.argv) > 4:
		recipe_file = sys.argv[4]
		if not os.path.exists(recipe_file):
			print("error: cannot open recipe path \"%s\"" % recipe_file)
			sys.exit(1)

	if recipe_file != None :
		if len (sys.argv) < 6:
			print("error: missing common directory path")
			sys.exit(1);
		if not os.path.exists(sys.argv[5]):
			print("error: cannot open source path \"%s\"" %sys.argv[5])
			sys.exit(1)
		commondir_path = os.path.abspath(sys.argv[5]) + "/"
		copy_files_from_recipe(WORK_DIR,commondir_path,recipe_file)
		#Remove recipe files
		for rfile in glob.glob(WORK_DIR+"*.recipe"):
			os.remove(rfile)

	os.chdir(WORK_DIR)

	compress_given_files(WORK_DIR)

	try:
		d = os.listdir(WORK_DIR)
	except OSError:
		print("error: cannot open path \"%s\"" % WORK_DIR)
		sys.exit(1)

	table = init_file_table(WORK_DIR, d)
	if table == None:
		sys.exit(2)
	
	buffer = pack_files(table)
	crc = crc32.crc32(buffer)
	backend_version = sys.argv[1]

	h = open(FTFS_IMG, "wb")
	h.write(MAGIC)
	h.write(struct.pack("I", crc & 0xffffffffL))
	h.write(struct.pack("I", 0 & 0xffffffffL))
	h.write(struct.pack("I", int(backend_version, 0) & 0xffffffffL))
	h.write(VERSION)
	h.write(buffer)
	h.close()

	st = os.stat(FTFS_IMG)

	if VERBOSE == 1:
		print("FTFS image created \"%s\"" % FTFS_IMG)
		print("Removing temp dir %s" % TMPDIR)

	# Navigate back to original directory so that the temporary
	# directory can be removed.
	os.chdir(CURDIR);
	shutil.rmtree(TMPDIR);

	sys.exit(0)
