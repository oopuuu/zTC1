#!/bin/sh

CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)

HOST_ARCHS="Cortex-M3 Cortex-M4"
BUS_LIST="SPI SDIO"
#BUS_LIST="SPI"

MODULES="3239 3165 3166 3238"
RF_MODULES="1062"


if [ ! -d “output” ]; then
  mkdir output
fi

for MODULE in $MODULES
do

echo Building $MODULE

rm -rf  build/snip_mxchipWNET-MXCHIP$MODULE-MiCORTOS-LwIP-SDIO

./make snip.mxchipWNET-MXCHIP$MODULE-MiCORTOS-LwIP-SDIO

#cp ./gen_library.sh build/snip_mxchipWNET-MXCHIP$MODULE-FreeRTOS-LwIP-SDIO/

cd build/snip_mxchipWNET-MXCHIP$MODULE-MiCORTOS-LwIP-SDIO/

rm -rf objs
mkdir objs
cd objs
find ../Modules -name "*.o" -exec cp {} ./ \;
rm -rf HardFault_handler.o MXCHIP*.o app_header.o config.o config_http_content.o crt0_GCC.o gpio_irq.o http_server.o mdns.o misc.o platform.o rtc.o spi_flash.o stm32*.o system_monitor.o vector_table_GCC.o watchdog.o wifi_image.o wwd_bus.o wwd_platform.o wwd_ring_buffer.o 
rm -rf $CURRENT_PATH/output/MiCO.$MODULE.GCC.a
$CURRENT_PATH/Tools/ARM_GNU/bin/OSX/arm-none-eabi-ar crv $CURRENT_PATH/output/MiCO.$MODULE.GCC.a *.o

cd $CURRENT_PATH

done



for MODULE in $RF_MODULES
do

for HOST_ARCH in $HOST_ARCHS
do

for BUS in $BUS_LIST
do

echo Building $MODULE $ARCH $BUS

if [ "$HOST_ARCH" = "Cortex-M3" ]
then
ARCH_STR="CM3"
fi

if [ "$HOST_ARCH" = "Cortex-M4" ]
then
ARCH_STR="CM4"
fi

rm -rf  build/snip_mxchipWNET-MXCHIP$MODULE$ARCH_STR-MiCORTOS-LwIP-$BUS

./make snip.mxchipWNET-MXCHIP$MODULE$ARCH_STR-MiCORTOS-LwIP-$BUS

#cp ./gen_library.sh build/snip_mxchipWNET-MXCHIP$MODULE$ARCH_STR-MiCORTOS-LwIP-$BUS/

cd build/snip_mxchipWNET-MXCHIP$MODULE$ARCH_STR-MiCORTOS-LwIP-$BUS/

rm -rf objs
mkdir objs
cd objs
find ../Modules -name "*.o" -exec cp {} ./ \;
rm -rf HardFault_handler.o MXCHIP*.o app_header.o config.o config_http_content.o crt0_GCC.o gpio_irq.o http_server.o mdns.o misc.o platform.o rtc.o spi_flash.o stm32*.o system_monitor.o vector_table_GCC.o watchdog.o wifi_image.o wwd_bus.o wwd_platform.o wwd_ring_buffer.o 
rm -rf $CURRENT_PATH/output/MiCO.$MODULE.$BUS.$HOST_ARCH.GCC.a
$CURRENT_PATH/Tools/ARM_GNU/bin/OSX/arm-none-eabi-ar crv $CURRENT_PATH/output/MiCO.$MODULE.$BUS.$HOST_ARCH.GCC.a *.o

cd $CURRENT_PATH

done
done
done

