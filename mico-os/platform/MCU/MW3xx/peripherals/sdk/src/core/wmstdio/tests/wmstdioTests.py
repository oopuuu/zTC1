import unittest
import pexpect
import time

global config
global mtfterm

class TestWMSTDIO(unittest.TestCase):

    def setUp(self):
	mtfterm.sendline("Module wmstdio registered");
        mtfterm.getprompt();

    #This test case will set help command and check help result.
    def testHelpCommand(self):
	mtfterm.sendline("help");
	mtfterm.expect(self, '');
	mtfterm.expect(self, 'help ');

