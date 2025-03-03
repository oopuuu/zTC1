import unittest
import time
from mtfterm import getConfigVar
import sys
import urllib2
import cfg

global config
global mtfterm
if cfg.CONFIG_OS_THREADX:
	class TestPwrmgr(unittest.TestCase):

		def setUp(self):
			# start each test disconnected
			mtfterm.sendline('wlan-disconnect')
			self.failIf(mtfterm.poll_with_timeout("wlan-stat", "Station disconnected (Active)", 30) != True,
					"Failed to initialize into disconnected state");

		def tearDown(self):
			# Be sure to leave everything in its default state so the other tests
			# pass!
			mtfterm.sendline('pm-on wlan');

		def testWlanOffOn(self):
			mtfterm.sendline('pm-off wlan')
			mtfterm.expect(self, "")
			mtfterm.sendline('pm-on wlan')
			mtfterm.expect(self, "")

		def testWlanOffWhileConnected(self):
			devNet = getConfigVar(config, 'devNetwork', 'devnet')
			mtfterm.netConnect(self, 'devnet', devNet)
			mtfterm.sendline('pm-off wlan')
			mtfterm.expect(self, 'Error: cannot power off "wlan".  Halt it first.')

		def testDeepsleepDuration(self):
			mtfterm.sendline('pm-deepsleep-set-dur 13')
			mtfterm.expect(self, "")
			mtfterm.sendline('pm-deepsleep-get-dur')
			mtfterm.sendline("Configured deep sleep duration is 13")

		def testDeepsleepWhileConnected(self):
			devNet = getConfigVar(config, 'devNetwork', 'devnet')
			mtfterm.netConnect(self, 'devnet', devNet)

			mtfterm.sendline('pm-deepsleep-set-dur 5')
			mtfterm.expect(self, "")
			mtfterm.sendline('pm-deepsleep-enter')
			mtfterm.expect(self, "Error: cannot enter deep sleep. Disconnect from WLAN first")

		def testDeepsleep(self):
			mtfterm.sendline('pm-deepsleep-set-dur 7')
			mtfterm.expect(self, "")

			start = time.time()
			mtfterm.sendline('pm-deepsleep-enter')

			# ensure console is unresponsive.  Take this as an indication that
			# we've entered deep sleep.  Note that we allow 10 seconds for the
			# device to come back, even though we only slept for 7.  The reason is
			# that the isAlive funtion may block for up to 10 seconds trying to
			# reach the device.
			while not mtfterm.isAlive():
				self.failIf(time.time() > start + 10, "Failed to wake from deepsleep")


