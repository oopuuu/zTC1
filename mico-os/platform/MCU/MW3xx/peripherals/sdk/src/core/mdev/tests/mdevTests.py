import unittest
import pexpect
import time
import cfg

global config
global mtfterm

class TestMDEV(unittest.TestCase):
	if cfg.CONFIG_OS_FREERTOS:
		def test1reg(self):
			mtfterm.sendline("mdev-test-register dev1");
			line = mtfterm.readline();
			if line == "Error":
				print ("Failed in registration of dev1");
			mtfterm.sendline("mdev-test-register dev2");
			line = mtfterm.readline()
			if line == "Error":
				print ("Failed in registration dev2");
			self.failIf(line == "Error", "Failed in registration");
		#Negative test case
		def test2RegwithSameName(self):
			mtfterm.sendline("mdev-test-register dev1");
			line = mtfterm.readline();
			if line == "Success":
				print ("Can't register device with same name");
			self.failIf(line == "Success", "Can't register device with same name");
		#Negative test case
		def test3RegwithoutName(self):
			mtfterm.sendline("mdev-test-register devNull");
			line = mtfterm.readline();
			if line == "Success":
				print ("Can't register device with same name");
			self.failIf(line == "Success", "Can't register device with same name");
		def test4open_unreg_dev(self):
			mtfterm.sendline("mdev-test-open dev3");
			line = mtfterm.readline();
			if line == "Success":
				print ("Failed in open");
			self.failIf(line == "Success", "Failed in opening unregistered device");
		def test5open(self):
			mtfterm.sendline("mdev-test-open dev1");
			line = mtfterm.readline();
			if line == "Error":
				print ("Failed in open");
			self.failIf(line == "Error", "Failed in opening device");
		def test6close(self):
			mtfterm.sendline("mdev-test-close dev1");
			line = mtfterm.readline()
			if line == "Error":
				print ("Failed in close");
			self.failIf(line == "Error", "Failed in closing device");
		def test7dereg(self):
			mtfterm.sendline("mdev-test-dereg dev1");
			line = mtfterm.readline()
			if line == "Error":
				print ("Failed in deregistration");
			self.failIf(line == "Error", "Failed to deregister");
		#Negative Test Case:
		def test8deregAgain(self):
			mtfterm.sendline("mdev-test-dereg dev1");
			line = mtfterm.readline()
			if line == "Success":
				print ("Failed in multiple deregistration");
			self.failIf(line == "Success", "Can't deregister again");
