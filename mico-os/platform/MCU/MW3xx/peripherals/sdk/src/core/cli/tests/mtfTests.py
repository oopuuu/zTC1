import unittest
import pexpect

global config
global mtfterm

class TestMTF(unittest.TestCase):
		
	def testHelpCommand(self):
		mtfterm.sendline('help')
		# check for a new line followed by the "help" command.  Additional help
		# text may follow depending on which commands are registered.
		line = mtfterm.readline()
		line = mtfterm.readline()
		mtfterm.getprompt()
		self.failIf(line != "help ", "Expected 'help' got '%s'" % line)

	def testNoSuchCommand(self):
		mtfterm.sendline('foobar')
		line = mtfterm.readline()
		self.failIf(line != "command 'foobar' not found",
					"Invalid command returned %s" % line)

	def testGetopt(self):
		mtfterm.sendline("getopt_test");
		mtfterm.expect(self, "");
		mtfterm.sendline("getopt_test foo");
		mtfterm.expect(self, "got extra arg foo");
		mtfterm.sendline("getopt_test -a -b -c foo");
		mtfterm.expect(self, "got option a");
		mtfterm.expect(self, "got option b");
		mtfterm.expect(self, "got option c with arg foo");
		mtfterm.sendline("getopt_test -x");
		mtfterm.expect(self, "ERROR: unexpected option: x");
		mtfterm.sendline("getopt_test -d -c foo -a");
		mtfterm.expect(self, "got option d");
		mtfterm.expect(self, "got option c with arg foo");
		mtfterm.expect(self, "got option a");
