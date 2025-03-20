set remotetimeout 20 
shell start /B D:\ProgramData\MiCoder_v1.3_Win32-64/MiCoder/OpenOCD/Win32/openocd_mico.exe -s ./ -f ./mico-os/makefiles/OpenOCD/interface/jlink_swd.cfg -f ./mico-os/makefiles/OpenOCD/mw3xx/mw3xx.cfg -f ./mico-os/makefiles/OpenOCD/mw3xx/mw3xx_gdb_jtag.cfg -l ./build/openocd_log.txt 
