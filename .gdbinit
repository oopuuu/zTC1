set remotetimeout 20 
shell start /B C:\Users\ooo\Desktop\MiCoder/OpenOCD/Win32/openocd_mico.exe -s ./ -f ./mico-os/makefiles/OpenOCD/interface/jlink_swd.cfg -f ./mico-os/makefiles/OpenOCD/mw3xx/mw3xx.cfg -f ./mico-os/makefiles/OpenOCD/mw3xx/mw3xx_gdb_jtag.cfg -l ./build/openocd_log.txt 
