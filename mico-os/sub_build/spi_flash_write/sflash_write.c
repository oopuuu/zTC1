#include <stdio.h>
#include <string.h>
#include "mico.h"
#include "platform_config.h"
#include "platform_init.h"
#include "platform_peripheral.h"
#include "platform_toolchain.h"
#include "linker_symbols.h"

/*
 * Test mode defines
 *
 * #define DEBUG_PRINT
 * #define WIPE_SFLASH
 * #define TEST_SFLASH_WRITE
 * #define TEST_SFLASH_READ
 * #define DEBUG_PRINT_READ_CONTENT
 * #define DEBUG_PRINT_VERIFY_READ_CONTENT
 */

/******************************************************
 *                      Macros
 ******************************************************/
//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define flash_program_log(format, ...)  custom_log("Flash", format, ##__VA_ARGS__)
#define DEBUG_PRINTF(x) printf x
#else
#define flash_program_log(format, ...)
#define DEBUG_PRINTF(x)
#endif /* ifdef DEBUG_PRINT */

/******************************************************
 *                    Constants
 ******************************************************/
/*
 * Commands to execute - bitwise OR together
 * TCL script write_sflash.tcl must match these defines
 */
#define MFG_SPI_FLASH_COMMAND_NONE                      (0x00000000)

#define MFG_SPI_FLASH_COMMAND_INITIAL_VERIFY            (0x00000001)
#define MFG_SPI_FLASH_COMMAND_ERASE_CHIP                (0x00000002)
#define MFG_SPI_FLASH_COMMAND_WRITE                     (0x00000004)
#define MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY         (0x00000008)
#define MFG_SPI_FLASH_COMMAND_VERIFY_CHIP_ERASURE       (0x00000010)
#define MFG_SPI_FLASH_COMMAND_READ                      (0x00000040)
#define MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED     (0x00000080)
#define MFG_SPI_FLASH_COMMAND_WRITE_DONE                (0x00000100)

#define WRITE_CHUNK_SIZE        (8*1024)  /* Writing in chunks is only needed to prevent reset by watchdog */
#define SECTOR_SIZE             (4096)

int partition_remapping[] =
{
    [BOOTLOADER_FIRMWARE_PARTITION_TCL]         = MICO_PARTITION_BOOTLOADER,
    [APPLICATION_FIRMWARE_PARTITION_TCL]        = MICO_PARTITION_APPLICATION,
    [ATE_FIRMWARE_PARTITION_TCL]                = MICO_PARTITION_ATE,
    [WIFI_FIRMWARE_PARTITION_TCL]               = MICO_PARTITION_RF_FIRMWARE,
    [PARAMETER_1_IMAGE_PARTITION_TCL]           = MICO_PARTITION_PARAMETER_1,
    [PARAMETER_2_IMAGE_PARTITION_TCL]           = MICO_PARTITION_PARAMETER_2,
#ifdef MICO_USE_BT_PARTITION
    [BT_PATCH_FIRMWARE_PARTITION_TCL]           = MICO_PARTITION_BT_FIRMWARE,
#else
    [BT_PATCH_FIRMWARE_PARTITION_TCL]           = -1,
#endif
    [FILESYSTEM_IMAGE_PARTITION_TCL]            = MICO_PARTITION_FILESYS,

};

/******************************************************
 *                   Enumerations
 ******************************************************/
/*
 * Result codes
 * TCL script write_sflash.tcl must match this enum
 */
typedef enum
{
    MFG_SPI_FLASH_RESULT_IN_PROGRESS = 0xffffffff,
    MFG_SPI_FLASH_RESULT_OK = 0,
    MFG_SPI_FLASH_RESULT_ERASE_FAILED = 1,
    MFG_SPI_FLASH_RESULT_VERIFY_AFTER_WRITE_FAILED = 2,
    MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_BUFFER = 3,
    MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_CHIP = 4,
    MFG_SPI_FLASH_RESULT_DCT_LOC_NOT_FOUND = 5,
    MFG_SPI_FLASH_RESULT_WRITE_FAILED = 6,
    MFG_SPI_FLASH_RESULT_READ_FAILED = 7,
    MFG_SPI_FLASH_RESULT_END = 0x7fffffff /* force to 32 bits */
} mfg_spi_flash_result_t;

//extern const platform_qspi_t platform_qspi_peripherals[];
//extern int init_qsflash( /*@out@*/ const platform_qspi_t* peripheral, uint32_t flash_length, /*@out@*/ sflash_handle_t* const handle, sflash_write_allowed_t write_allowed_in );
//extern int qsflash_write( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, unsigned int size );
//extern int qsflash_read( const sflash_handle_t* const handle, unsigned long device_address, void* const data_addr, unsigned int size );
//extern int qsflash_sector_erase ( unsigned long device_address );
extern const mico_logic_partition_t mico_partitions[];
extern const platform_flash_t platform_flash_peripherals[];

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
/*
 * TCL script write_sflash.tcl must match this structure
 */
typedef struct
{
    void * entry_point;
    void * stack_addr;
    unsigned long data_buffer_size;
} data_config_area_t;

/*
 * TCL script write_sflash.tcl must match this structure
 */

typedef struct
{
    unsigned long size;
    unsigned long partition;
    unsigned long partition_offset;
    unsigned long command;
    mfg_spi_flash_result_t result;
    unsigned char data[__JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__ ];
} data_transfer_area_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/
#ifdef WIPE_SFLASH
static void add_wipe_data( void );
#elif defined( TEST_SFLASH_WRITE )
static void add_test_data( void );
#elif defined( TEST_SFLASH_READ )
static void read_test_data2( void );
#endif /* ifdef TEST_SFLASH_READ */

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t Rx_Buffer[SECTOR_SIZE + 10]; /* A temporary buffer used for reading data from the Serial flash when performing verification */

/******************************************************************************
 * This structure provides configuration parameters, and communication area
 * to the TCL OpenOCD script
 * It will be located at the start of RAM by the linker script
 *****************************************************************************/
#if defined(__ICCARM__)
/* IAR specific */
#pragma section= "data_config_section"
const data_config_area_t data_config @ "data_config_section";
#endif /* #if defined(__ICCARM__) */
const data_config_area_t data_config =
    {
        .entry_point = (void*) ENTRY_ADDRESS,
        .stack_addr = &link_stack_end,
        .data_buffer_size = __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__,
    };

/******************************************************************************
 * This structure provides a transfer area for communications with the
 * TCL OpenOCD script
 * It will be located immediately after the data_config structure at the
 * start of RAM by the linker script
 *****************************************************************************/
#if defined (__ICCARM__)
/* IAR specific */
#pragma section= "data_transfer_section"
data_transfer_area_t data_transfer @ "data_transfer_section";
#else /* #if defined (__ICCARM__) */
data_transfer_area_t data_transfer;
#endif /* #if defined (__ICCARM__) */

/******************************************************
 *               Function Definitions
 ******************************************************/

int not_erased (const void *a1, size_t size)
{
    uint8_t* cmp_ptr = (uint8_t *)a1;
    uint8_t* end_ptr = (uint8_t *)a1 + size;

    while ( ( cmp_ptr < end_ptr ) && ( *cmp_ptr == (uint8_t) 0xff ) )
    {
        cmp_ptr++;
    }

    /* No not erase if already all 0xFF*/
    if ( cmp_ptr < end_ptr )
        return (int)(cmp_ptr - (uint8_t *)a1 + 1);
    else
        return 0;
}

int main( void )
{
    unsigned long pos;
//    sflash_handle_t sflash_handle;
    unsigned long chip_size = 0;
    bool erase_once = true;
#if defined ( __IAR_SYSTEMS_ICC__ )
    /* IAR allows init functions in __low_level_init(), but it is run before global
     * variables have been initialised, so the following init still needs to be done
     * When using GCC, this is done in crt0_GCC.c
     */
    platform_init_mcu_infrastructure( );
    platform_init_external_devices( );
#endif /* #elif defined ( __IAR_SYSTEMS_ICC__ ) */
    
    //NoOS_setup_timing( );

#ifdef WIPE_SFLASH
    add_wipe_data( );
#elif defined( TEST_SFLASH_WRITE )
    add_test_data( );
#elif defined( TEST_SFLASH_READ )
    read_test_data2( );
#endif /* ifdef TEST_SFLASH_READ */

#ifdef PLATFORM_HAS_OTP
    platform_otp_setup( );
#endif /* ifdef PLATFORM_HAS_OTP */

    /* loop forever */
    while ( 1 == 1 )
    {
        flash_program_log( "Waiting for command" );

        /* wait for a command to be written. */
        /*@-infloopsuncon@*//* Lint: Loop variable is modified by JTAG writing directly into the data transfer memory */
        while ( data_transfer.command == MFG_SPI_FLASH_COMMAND_NONE )
        {
            (void) platform_watchdog_kick( );
        }
        /*@-infloopsuncon@*/

        data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;

        flash_program_log( "Received command: %s%s%s%s%s%s%s",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_INITIAL_VERIFY )? "INITIAL_VERIFY " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_ERASE_CHIP )? "ERASE_CHIP " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE )? "WRITE " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY )? "POST_WRITE_VERIFY " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_VERIFY_CHIP_ERASURE )? "VERIFY_CHIP_ERASURE " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_READ )? "READ " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED)? "WRTIE_ERASE_IF_NEEDED " : "",
            ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE_DONE)? "MFG_SPI_FLASH_COMMAND_WRITE_DONE " : ""
        );
        flash_program_log( "Destination partition: %u, Destination address: %lx", partition_remapping[data_transfer.partition], data_transfer.partition_offset );
        flash_program_log( "Size: %lu", data_transfer.size );

        /* Check the data size is sane - cannot be bigger than the data storage area */
        if ( data_transfer.size > (unsigned long) __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__ )
        {
            data_transfer.result = MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_BUFFER;
            flash_program_log( "Size %lu too big to for storage area %d - aborting!", data_transfer.size, __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__);
            goto back_to_idle;
        }

        if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_READ) != 0 )
        {
            flash_program_log( "Reading data!" );

            /* Read data from SPI FLASH memory */
            pos = 0;
            unsigned long position = 0;
            position = pos + data_transfer.partition_offset;
            if ( 0
                != MicoFlashRead( partition_remapping[data_transfer.partition], &position, data_transfer.data,
                                  (unsigned int) data_transfer.size ) )
            {
                /* Read failed */
                data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                flash_program_log( "Read error - abort!" );
                goto back_to_idle;
            }
#ifdef DEBUG_PRINT_READ_CONTENT
            {
                int i;
                for( i = 0;i < data_transfer.size; i++ )
                {
                    DEBUG_PRINTF( ( "%02X ", data_transfer.data[i]));
                    if( ( i & 0xf ) == 0xf )
                    {
                        DEBUG_PRINTF( ( "\r\n") );
                    }
                }
            }
#endif /* DEBUG_PRINT_READ_CONTENT */

            data_transfer.result = MFG_SPI_FLASH_RESULT_OK;
            flash_program_log( "Finished Read!" );
            goto back_to_idle;
        }

        /* Check the data will fit on the sflash chip */
        if ( (chip_size != 0) && (data_transfer.size + data_transfer.partition_offset > chip_size) )
        {
            data_transfer.result = MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_CHIP;
            flash_program_log( "Size (%lu from address %lu) too big to fit on partition (%lu) - aborting!", data_transfer.size, data_transfer.partition_offset, chip_size );
            goto back_to_idle;
        }
        if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_INITIAL_VERIFY) != 0 )
        {
            flash_program_log( "Verifying existing data!" );

            /* Read data from SPI FLASH memory */
            pos = 0;
            while ( pos < data_transfer.size )
            {
                unsigned int read_size =
                    (data_transfer.size - pos > (unsigned long) sizeof(Rx_Buffer)) ?
                        (unsigned int) sizeof(Rx_Buffer) :
                        (unsigned int) (data_transfer.size - pos);
                unsigned long position = 0;
                position = pos + data_transfer.partition_offset;
                if ( 0
                    != MicoFlashRead( partition_remapping[data_transfer.partition], &position, Rx_Buffer,
                                      read_size ) )
                {
                    /* Verify Error - Chip not erased properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                    flash_program_log( "Read error - abort!" );
                    goto back_to_idle;
                }
                if ( 0 != memcmp( Rx_Buffer, &data_transfer.data[pos], (size_t) read_size ) )
                {
                    /* Existing data different */
                    flash_program_log( "Existing data is different - stop verification" );
                    /*@innerbreak@*//* Only break out of inner-most loop */
                    break;
                }
                pos += read_size;
                (void) platform_watchdog_kick( );
            }
            if ( pos >= data_transfer.size )
            {
                /* Existing data matches */
                /* No write required */
                data_transfer.result = MFG_SPI_FLASH_RESULT_OK;
                flash_program_log( "Existing data matches - successfully aborting!" );
                goto back_to_idle;
            }
        }

        if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_ERASE_CHIP) != 0 )
        {
            flash_program_log( "Erasing entire chip" );

            /* Erase the serial flash chip */
            /*delete for test*/
            if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_VERIFY_CHIP_ERASURE) != 0 )
            {
                flash_program_log( "Verifying erasure of entire chip" );

                /* Verify Erasure */
                pos = 0;
                while ( pos < chip_size )
                {
                    uint8_t* cmp_ptr;
                    uint8_t* end_ptr;
                    unsigned int read_size;

                    read_size =
                        (chip_size - pos > (unsigned long) sizeof(Rx_Buffer)) ?
                            (unsigned int) sizeof(Rx_Buffer) : (unsigned int) (chip_size - pos);
                    if ( 0
                        != MicoFlashRead( partition_remapping[data_transfer.partition], &pos, Rx_Buffer, read_size ) )
                    {
                        /* Verify Error - Chip not erased properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                        flash_program_log( "Read error - abort!" );
                        goto back_to_idle;
                    }
                    cmp_ptr = Rx_Buffer;
                    end_ptr = &Rx_Buffer[read_size];
                    while ( (cmp_ptr < end_ptr) && (*cmp_ptr == (uint8_t) 0xff) )
                    {
                        cmp_ptr++;
                    }
                    if ( cmp_ptr < end_ptr )
                    {
                        /* Verify Error - Chip not erased properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_ERASE_FAILED;
                        flash_program_log( "Chip was not erased properly - abort!" );
                        goto back_to_idle;
                    }
                    pos += read_size;
                    (void) platform_watchdog_kick( );
                }
            }
        }

        if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE) != 0 )
        {
            flash_program_log( "Writing location" );

            /* Write the WLAN firmware into memory */
            pos = 0;
            while ( pos < data_transfer.size )
            {
                unsigned int write_size =
                    (data_transfer.size - pos > (unsigned long) WRITE_CHUNK_SIZE) ?
                        (unsigned int) WRITE_CHUNK_SIZE : (unsigned int) (data_transfer.size - pos);
                unsigned long position = 0;
                position = pos + data_transfer.partition_offset;
                if ( 0
                    != MicoFlashWrite( partition_remapping[data_transfer.partition], &position,
                                       &data_transfer.data[pos], write_size ) )
                {
                    /* Verify Error - Chip not erased properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_WRITE_FAILED;
                    flash_program_log( "Write error - abort!" );
                    goto back_to_idle;
                }
                pos += write_size;
                (void) platform_watchdog_kick( );
            }

        }

        if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED) != 0 )
        {
            mico_logic_partition_t *partition_info;
            uint32_t compare_start = (uint32_t) data_transfer.partition_offset % SECTOR_SIZE;
            int32_t size_left_to_compare = (int32_t) data_transfer.size;
            unsigned char* cmp_ptr = data_transfer.data;
            pos = (data_transfer.partition_offset / SECTOR_SIZE) * SECTOR_SIZE;


            /* Force erase embedded flash once before program */
            partition_info = MicoFlashGetInfo( partition_remapping[data_transfer.partition] );
            if(  platform_flash_peripherals[partition_info->partition_owner].flash_type == FLASH_TYPE_EMBEDDED && erase_once )
            {
                flash_program_log( "Partition erase!" );
                MicoFlashErase( partition_remapping[data_transfer.partition], 0x0, partition_info->partition_length);
                erase_once = false;
            }

            flash_program_log( "Verifying existing data!" );
            /* Read data from SPI FLASH memory */
            while ( size_left_to_compare > 0 )
            {
                uint32_t cmp_len;
                unsigned long position;
                position = pos;

                if ( 0
                    != MicoFlashRead( partition_remapping[data_transfer.partition], &position, Rx_Buffer,
                                      (unsigned int) SECTOR_SIZE ) )
                {
                    /* Verify Error - Chip not read properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                    flash_program_log( "Read error - abort!" );
                    goto back_to_idle;
                }
                cmp_len = (uint32_t) MIN( (int32_t) ( SECTOR_SIZE - compare_start ), size_left_to_compare );
#ifndef ALWAYS_ERASE_SECTOR
                if ( 0 != memcmp( &Rx_Buffer[compare_start], cmp_ptr, cmp_len ) )
#endif /* ALWAYS_ERASE_SECTOR */
                {
                    flash_program_log("Need to erase if non 0xFF");
                    /* No not erase if already all 0xFF */
                    if ( not_erased( &Rx_Buffer[compare_start], cmp_len ) )
                    {
                        /* Existing data different */
                        uint32_t offset;
                        flash_program_log( "Erasing sector 0x%lx", pos );
                        offset = pos;
                        if ( 0 != MicoFlashErase( partition_remapping[data_transfer.partition], offset, SECTOR_SIZE ) )
                        {
                            /* Sector Erase Error - Chip not erased properly */
                            data_transfer.result = MFG_SPI_FLASH_RESULT_ERASE_FAILED;
                            flash_program_log( "Sector erase error - abort!" );
                            goto back_to_idle;
                        }
                    }
#ifdef VERIFY_ERASE_SECTOR
                    {
#ifdef DEBUG
                        memset(Rx_Buffer,0xa5,SECTOR_SIZE);
#endif /* DEBUG */
                        if ( 0 != MicoFlashRead( partition_remapping[data_transfer.partition], &position, Rx_Buffer,
                                                 (unsigned int) SECTOR_SIZE ) )
                        {
                            /* Verify Error - Chip not read properly */
                            data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                            flash_program_log( "Read error - abort!" );
                            goto back_to_idle;
                        }

                        if ( not_erased( Rx_Buffer, SECTOR_SIZE ) )
                        {
                            /* Verify Error - sector not erased properly */
                            data_transfer.result = MFG_SPI_FLASH_RESULT_ERASE_FAILED;
                            flash_program_log( "Sector was not erased properly - abort!" );
                            flash_program_log( "First bystes read: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", Rx_Buffer[0], Rx_Buffer[1], Rx_Buffer[2], Rx_Buffer[3],
                                    Rx_Buffer[4], Rx_Buffer[5], Rx_Buffer[6], Rx_Buffer[7] );
                            goto back_to_idle;
                        }
                    }
#endif /* ifdef VERIFY_ERASE_SECTOR */

                    flash_program_log( "Writing address 0x%lx", pos );
                    memcpy( &Rx_Buffer[compare_start], cmp_ptr, cmp_len );
                    unsigned long position;
                    position = pos;
                    flash_program_log("position is 0x%lx",position);
                    if ( 0
                        != MicoFlashWrite( partition_remapping[data_transfer.partition], &position, Rx_Buffer,
                                           (unsigned int) SECTOR_SIZE ) )
                    {
                        /* Write Error - Chip not written properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_WRITE_FAILED;
                        flash_program_log( "Write error - abort!" );
                        goto back_to_idle;
                    }
                    flash_program_log("Write success");
                }
                cmp_ptr += cmp_len;
                size_left_to_compare -= cmp_len;
                compare_start = 0;

                pos += SECTOR_SIZE;
                (void) platform_watchdog_kick( );
            }
        }

        if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY) != 0 )
        {
            flash_program_log( "Verifying after write" );

            /* Read data from SPI FLASH memory */
            pos = 0;
            while ( pos < data_transfer.size )
            {
                unsigned int read_size =
                    (data_transfer.size - pos > (unsigned long) sizeof(Rx_Buffer)) ?
                        (unsigned int) sizeof(Rx_Buffer) : data_transfer.size - pos;
                unsigned long position;
                position = pos + data_transfer.partition_offset;
                if ( 0
                    != MicoFlashRead( partition_remapping[data_transfer.partition], &position, Rx_Buffer,
                                      read_size ) )
                {
                    /* Verify Error - Chip not erased properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                    flash_program_log( "Read error - abort!" );
                    goto back_to_idle;
                }
#ifdef DEBUG_PRINT_VERIFY_READ_CONTENT
                {
                    int i;
                    for( i = 0;i < read_size; i++ )
                    {
                        DEBUG_PRINTF(( "%02X %02X ", Rx_Buffer[i], data_transfer.data[i]));
                        if( ( i & 0xf ) == 0xf )
                        {
                            DEBUG_PRINTF( ( "\r\n") );
                        }
                    }
                }
#endif /* DEBUG_PRINT_VERIFY_READ_CONTENT */
                if ( 0 != memcmp( Rx_Buffer, &data_transfer.data[pos], (size_t) read_size ) )
                {
                    /* Verify Error - Read data different to written data */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_VERIFY_AFTER_WRITE_FAILED;
                    flash_program_log( "Verify error - Data was not written successfully - abort!Pos=%lx", pos );
                    goto back_to_idle;
                }
                pos += read_size;
                (void) platform_watchdog_kick( );
            }
        }
       if ( (data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE_DONE) != 0 )
       {
           erase_once = true;
           flash_program_log( " Write operation finished at partition %d", APPLICATION_FIRMWARE_PARTITION_TCL );
       }
        /* OK! */
        data_transfer.result = MFG_SPI_FLASH_RESULT_OK;

        back_to_idle:
        data_transfer.command = MFG_SPI_FLASH_COMMAND_NONE;
        data_transfer.partition_offset = 0;
        data_transfer.size = 0;
    }

    return 0;
}

#if defined(__ICCARM__)
#pragma section="CSTACK"
__root void _mico_iar_program_start(void)
{
    /* When the execution of the program is initiated from an external debugger */
    /* it will perform a reset of the CPU followed by halt and set a program counter to program entry function __iar_program_start leaving */
    /* SP unchanged */

    /* Consequently, the SP value will be set to the value which was read from address 0x00000000(applicable for CM3) */
    /* For apps which have an interrupt vector table placed at the start of the flash, the value of the SP will */
    /* contain correct value, however for apps which have interrupt vectors shifted to a different memory location, */
    /* the SP will contain garbage. On entry we must call this function which will set the SP to point to the end */
    /* of the CSTACK section */
    iar_set_msp(__section_end("CSTACK"));
}

#endif

#ifdef WIPE_SFLASH

static void add_wipe_data( void )
{
    data_transfer.command = MFG_SPI_FLASH_COMMAND_ERASE_CHIP;
    data_transfer.sflash_address = 0;
    data_transfer.size = 0;
    data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;
}

#elif defined( TEST_SFLASH_WRITE )

static void add_test_data( void )
{
    const char test_data[] = /* 0 */
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    /* 1024 */
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    /* 2048 */
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    /* 3072 */
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    /* 4096 */
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef012";
    /* 4643 */

    data_transfer.command = MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED | MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY;
//    data_transfer.command |=  MFG_SPI_FLASH_COMMAND_ERASE_CHIP;
    data_transfer.partition = APPLICATION_FIRMWARE_PARTITION_TCL;
    data_transfer.partition_offset = 0x00;
    data_transfer.size = sizeof(test_data)-1;
    data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;
    memcpy( data_transfer.data, test_data, sizeof(test_data)-1);
}

#elif defined( TEST_SFLASH_READ )

static void read_test_data2( void )
{
    data_transfer.command = MFG_SPI_FLASH_COMMAND_READ;
    data_transfer.partition = APPLICATION_FIRMWARE_PARTITION_TCL;
    data_transfer.partition_offset = 0x00;
    data_transfer.size = 0x1000;
    data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;
}

#endif /* ifdef TEST_SFLASH_READ */

