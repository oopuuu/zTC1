import unittest
import pexpect
import re

global config
global mtfterm

class TestFTFS(unittest.TestCase):

	def testsListDirectoryCommand(self):
		mtfterm.sendline('ftfs-ls')
                files = ["header.txt", "header.html.gz", "dostextfile.txt", "body.html.gz", "body.txt", "testecho.shtml", "footer.txt", "testvirtual.shtml", "testpost.shtml", "testshtml.shtml", "footer.html.gz", "unixtextfile.txt", "404.html.gz", "testget.txt", "index.html.gz"]
		line = mtfterm.readline()
		while line != "":
			self.failIf(line.split(' ')[0].replace('\"', '') not in files,
				"Unexpected file in ls output: " + line)
			line = mtfterm.readline()			

	def testsCatCommand(self):
		mtfterm.sendline('ftfs-cat dostextfile.txt')
		# chomp the file name and size
		line = mtfterm.readline()
		mtfterm.expect(self, '"This file contains')
		mtfterm.expect(self, 'DOS-style')
		mtfterm.expect(self, 'Line-endings')
		mtfterm.expect(self, '"')

	def testsUNIXCatCommand(self):
		mtfterm.sendline('ftfs-cat unixtextfile.txt')
		# chomp the file name and size
		line = mtfterm.readline()
		mtfterm.expect(self, '"This file contains\n')
		mtfterm.expect(self, 'UNIX-style\n')
		mtfterm.expect(self, 'Line-endings\n')
		mtfterm.expect(self, '"')
		mtfterm.sendline('ftfs-cat -u unixtextfile.txt')
		# chomp the file name and size
		line = mtfterm.readline()
		mtfterm.expect(self, '"This file contains')
		mtfterm.expect(self, 'UNIX-style')
		mtfterm.expect(self, 'Line-endings')
		mtfterm.expect(self, '"')
