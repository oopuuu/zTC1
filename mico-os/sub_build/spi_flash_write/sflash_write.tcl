#
# sflash_write.tcl
#
# This TCL OpenOCD script runs on a PC and communicates with the embedded
# sflash_write app to allow the PC to write data into a serial flash chip
# attached to the target processor
#
# Usage example:
#
# source [find mfg_spi_flash/write_sflash.tcl]
# sflash_init "MK3165@debug"
# sflash_write_file "example.bin" 0x10 "MK3165@debug" 1
# shutdown
#
###############################################################################

# CHIP_RAM_START must be supplied by target specific TCL script
set MemoryStart $CHIP_RAM_START

###############################################################################
#
# These variables must match the ones in Apps/waf/sflash_write/sflash_write.c
#
# They rely on the linker script placing the data_config_area_t and
# data_transfer_area_t structures at the start of RAM
#
###############################################################################

# This must match data_config_area_t
set entry_address_loc  [expr $MemoryStart + 0x00 ]
set stack_address_loc  [expr $MemoryStart + 0x04 ]
set buffer_size_loc    [expr $MemoryStart + 0x08 ]

# This must match data_transfer_area_t
set data_size_loc      [expr $MemoryStart + 0x0C ]
set dest_partition_loc [expr $MemoryStart + 0x10 ]
set dest_address_loc   [expr $MemoryStart + 0x14 ]
set command_loc        [expr $MemoryStart + 0x18 ]
set result_loc         [expr $MemoryStart + 0x1C ]
set data_loc           [expr $MemoryStart + 0x20 ]


# These must match the MFG_SPI_FLASH_COMMAND defines
set COMMAND_INITIAL_VERIFY            (0x01)
set COMMAND_ERASE                     (0x02)
set COMMAND_WRITE                     (0x04)
set COMMAND_POST_WRITE_VERIFY         (0x08)
set COMMAND_VERIFY_CHIP_ERASURE       (0x10)
set COMMAND_WRITE_DCT                 (0x20)
set COMMAND_READ                      (0x40)
set COMMAND_WRITE_ERASE_IF_NEEDED     (0x80)
set COMMAND_WRITE_DONE                (0x100)

# These must match the mfg_spi_flash_result_t enum
set RESULT(0xffffffff)      "In Progress"
set RESULT(4294967295)      "In Progress"
set RESULT(0)               "OK"
set RESULT(1)               "Erase Failed"
set RESULT(2)               "Verify after write failed"
set RESULT(3)               "Size too big for buffer"
set RESULT(4)               "Size too big for chip"
set RESULT(5)               "DCT location not found - has factory reset app been written?"
set RESULT(6)               "Error during Write"
set RESULT(7)               "Error during Read"


###############################################################################
# memread32
#
# Helper function that reads a 32 bit value from RAM and returns it
#
# address - the RAM address to read from
###############################################################################
proc memread32 {address resume_required} {
    if { $resume_required == 1 } {
        halt
    }
    mem2array memar 32 $address 1
    if { $resume_required == 1} {
        resume
    }
    return $memar(0)
}

###############################################################################
# load_image_bin
#
# Loads part of a binary file into RAM
#
# fname   - filename of binary image file
# foffset - offset from the start of the binary file where data will be read
# address - the destination RAM address
# length  - number of bytes to transfer
###############################################################################
proc load_image_bin {fname foffset address length } {
  # Load data from fname filename at foffset offset to
  # target at address. Load at most length bytes.
  puts "loadimage address $address foffset $foffset $length"
  load_image $fname [expr $address - $foffset] bin $address $length
}



###############################################################################
# sflash_init
#
# Prepares for writing to serial flashby loading the sflash_write app into
# memory and setting it running.
# This function assumes the following target has already been built:
#      waf.sflash_write-NoOS-<PlatBusDebug>
#
# PlatBusDebug   - The platform, bus and debug part of the build target
# init4390       - run initialisation for the 4390
###############################################################################
proc sflash_init { PlatBusDebug } {
    global entry_address_loc
    global stack_address_loc
    global buffer_size_loc
    global entry_address
    global stack_address
    global buffer_size
    
    init
    reset init
    reset halt
    
    #load_image build/sub_build.spi_flash_write@NoRTOS@$PlatBusDebug/binary/sub_build.spi_flash_write@NoRTOS@$PlatBusDebug.elf
    if {[file exists mico-os/board/$PlatBusDebug/flash_prog.elf]} {
        load_image mico-os/board/$PlatBusDebug/flash_prog.elf
    } else {
        if {[file exists board/$PlatBusDebug/flash_prog.elf]} {
            load_image board/$PlatBusDebug/flash_prog.elf
        } else {
            error "Error: Cann't find flash_prog.elf in $PlatBusDebug directory"
            exit -1;            
        }
    }
    
    #mdw 0x000D0000 100
    #mdw $entry_address_loc 100

    set entry_address [memread32 $entry_address_loc 0]
    set stack_address [memread32 $stack_address_loc 0]
    set buffer_size   [memread32 $buffer_size_loc 0]

    puts "entry_address= $entry_address"
    puts "stack_address= $stack_address"
    puts "buffer_size= $buffer_size"
    if { $buffer_size == 0 } {
        error "Error: Buffer size read from address $buffer_size_loc on target is zero"
        exit -1;
    }


    
    # Setup start address
    reg pc $entry_address
    reg sp $stack_address
    
    resume
    sleep 500
}


###############################################################################
# program_sflash
#
# Executes a serial command by communicating to the sflash_write app
#
# fname    - filename of binary image file (if command requires it)
# foffset  - offset from the start of the binary file where data will be read  (if command requires it)
# dataSize - number of bytes to transfer (if command requires it)
# partition- the destination flash partition  (if command requires it)
# destaddr - the destination offset address on a partition (if command requires it)
# cmd      - The commmand to execute (see list above)
###############################################################################
proc program_sflash { filename foffset dataSize partition destaddr cmd } {
    global PlatBusDebug MemoryStart data_size_loc dest_partition_loc data_loc dest_address_loc command_loc result_loc entry_address RESULT  entry_address_loc

    halt
    # Load the binary data into the RAM
    if { ( $dataSize != 0 ) && ( $filename != "" ) } {
       load_image_bin $filename $foffset $data_loc $dataSize
    }

    # Write the details of the data

    mww $data_size_loc      $dataSize
    mww $dest_partition_loc $partition
    mww $dest_address_loc   $destaddr
    mww $result_loc         0xffffffff

	puts "========================dest_address_loc= $destaddr"
    #mdw $entry_address_loc 100

    # Write the command - This causes the writing to start
    mww $command_loc      $cmd
    resume

    set resultval 0xffffffff
    set loops  0
    while { ($resultval == 0xffffffff) && ( $loops < 100 ) } {
     sleep 100
     set resultval [memread32 $result_loc 1]
     incr loops
    }

    puts "****************** Result: $RESULT($resultval)"

    if { $resultval != 0 } {
        halt
        reg
        error "program_sflash error, Result: $RESULT($resultval)"
        exit -1;
    }

}

###############################################################################
# program_sflash_done
#
# Notice flash write is finished, perfor some post actions
#
###############################################################################
proc program_sflash_done { partition } {
    global  RESULT dest_partition_loc result_loc command_loc COMMAND_WRITE_DONE

    halt
   
    mww $dest_partition_loc $partition
    mww $command_loc    [expr $COMMAND_WRITE_DONE]
    resume

    set resultval 0xffffffff
    set loops  0
    while { ($resultval == 0xffffffff) && ( $loops < 100 ) } {
     sleep 100
     set resultval [memread32 $result_loc 1]
     incr loops
    }

    puts "****************** Result: $RESULT($resultval)"

    if { $resultval != 0 } {
        halt
        reg
        error "program_sflash_done error, Result: $RESULT($resultval)"
        exit -1;
    }

}


###############################################################################
# sflash_write_file
#
# Writes an entire binary image file to a serial flash address
# This function assumes the following target has already been built:
#      waf.sflash_write-NoOS-<PlatBusDebug>
#
# filename     - filename of binary image file
# partition    - the destination flash partition
# destAddress  - the destination flash partition offset address
# PlatBusDebug - The platform, bus and debug part of the build target
# erasechip    - If 1, Erase the partition before writing.
# init4390     - run initialisation for the 4390
###############################################################################
proc sflash_write_file { filename partition destAddress PlatBusDebug erasechip } {
    global COMMAND_ERASE COMMAND_INITIAL_VERIFY COMMAND_WRITE COMMAND_POST_WRITE_VERIFY buffer_size COMMAND_WRITE_ERASE_IF_NEEDED

    sflash_init $PlatBusDebug
    
    set binDataSize [file size $filename]
    # set erase_command_val [expr $COMMAND_ERASE ]
    set write_command_val [expr $COMMAND_WRITE_ERASE_IF_NEEDED | $COMMAND_POST_WRITE_VERIFY ]
    set pos 0

    # if { $erasechip } {
        # puts "Erasing partition $partition"
        # program_sflash $filename $pos 0 $partition $destAddress $erase_command_val
        # puts "Chip Erase Done"
    # }

    puts "Total write size is $binDataSize to partition $partition"
    while { $pos < $binDataSize } {
        if { ($binDataSize - $pos) <  $buffer_size } {
            set writesize [expr ($binDataSize - $pos)]
        } else {
            set writesize $buffer_size
        }
        puts "writing $writesize bytes at [expr $destAddress + $pos]"
        program_sflash $filename $pos $writesize $partition [expr $destAddress + $pos] $write_command_val
        set pos [expr $pos + $writesize]
    }
    
    program_sflash_done $partition
}



###############################################################################
# sflash_read_file
#
# Reads data from a serial flash address
# This function assumes the following target has already been built:
#      waf.sflash_write-NoOS-<PlatBusDebug>
#
# filename     - output filename for binary image file
# srcAddress   - the destination serial flash address
# PlatBusDebug - The platform, bus and debug part of the build target
# length       - number of bytes to read
# init4390     - run initialisation for the 4390
###############################################################################
proc sflash_read_file { filename srcAddress PlatBusDebug length } {
    global COMMAND_ERASE COMMAND_INITIAL_VERIFY COMMAND_WRITE COMMAND_POST_WRITE_VERIFY buffer_size COMMAND_READ data_loc

    sflash_init $PlatBusDebug
    set temp_file "temp.bin"
    exec tools/common/Win32/echo -n > $filename

    set read_command_val [expr $COMMAND_READ ]
    set pos 0

    puts "Total read size is $length"
    while { $pos < $length } {
        if { ($length - $pos) <  $buffer_size } {
            set readsize [expr ($length - $pos)]
        } else {
            set readsize $buffer_size
        }
        puts "reading $readsize bytes from [expr $srcAddress + $pos]"
        program_sflash "" $pos $readsize [expr $srcAddress + $pos] $read_command_val
#        mem2array memar 8 [expr $data_loc-8] 1024
#        puts "$memar(0) $memar(1) $memar(2) $memar(3) $memar(4) $memar(5) $memar(6) $memar(7) $memar(8)"
        puts "dumping image from $data_loc $readsize"
        halt
        dump_image  $temp_file $data_loc $readsize

        exec cat $temp_file >> $filename
        set pos [expr $pos + $readsize]
    }
}



proc sflash_erase { PlatBusDebug } {
    global COMMAND_ERASE COMMAND_INITIAL_VERIFY COMMAND_WRITE COMMAND_POST_WRITE_VERIFY buffer_size COMMAND_WRITE_ERASE_IF_NEEDED

    sflash_init $PlatBusDebug

    set erase_command_val [expr $COMMAND_ERASE ]

    puts "Erasing Chip"
    program_sflash "" 0 0 0 $erase_command_val
    puts "Chip Erase Done"
}
