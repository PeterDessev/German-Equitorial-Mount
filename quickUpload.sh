PORT=$1

if [ "$PORT" == "" ]; then
    echo Please enter a port
    exit 1
fi

if [ "$(ls | grep bin)" == "" ]; then
    mkdir bin
fi

# -Wall -Wextra
avr-g++ -MMD -c -D__PROG_TYPES_COMPAT__ -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=1819 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR -DARDUINO_BOARD=AVR_NANO -DARDUINO_VARIANT=eightanaloginputs -I include -I Arduino/hardware/arduino/avr/cores/arduino -I Arduino/hardware/arduino/avr/cores/arduino/api -I Arduino/hardware/arduino/avr/variants/eightanaloginputs -I Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src -I Arduino/hardware/arduino/avr/libraries/SPI/src/ -ffunction-sections -fdata-sections -Os -pedantic -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color=always src/main.cpp  -o bin/main.cpp.o

avr-g++ -MMD -c -D__PROG_TYPES_COMPAT__ -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=1819 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR -DARDUINO_BOARD=AVR_NANO -DARDUINO_VARIANT=eightanaloginputs -I include -I Arduino/hardware/arduino/avr/cores/arduino -I Arduino/hardware/arduino/avr/cores/arduino/api -I Arduino/hardware/arduino/avr/variants/eightanaloginputs -I Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src -I Arduino/hardware/arduino/avr/libraries/SPI/src/ -ffunction-sections -fdata-sections -Os -pedantic -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color=always src/uart.cpp  -o bin/uart.cpp.o

# avr-g++ -MMD -c -D__PROG_TYPES_COMPAT__ -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=1819 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR -DARDUINO_BOARD=AVR_NANO -DARDUINO_VARIANT=eightanaloginputs -I Arduino/hardware/arduino/avr/cores/arduino -I Arduino/hardware/arduino/avr/cores/arduino/api -I Arduino/hardware/arduino/avr/variants/eightanaloginputs -ffunction-sections -fdata-sections -Os -pedantic -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color=always Arduino/hardware/arduino/avr/cores/arduino/HardwareSerial.cpp  -o bin/HardwareSerial.cpp.o

avr-gcc -flto -fuse-linker-plugin -mmcu=atmega328p -Wl,--gc-sections -Os -o ./bin/EquitorialMountDriver_.elf ./bin/main.cpp.o ./bin/core.a ./bin/SoftwareSerial.cpp.o ./bin/core_arch.a ./bin/HardwareSerial.cpp.o ./bin/uart.cpp.o -lc -lm

avr-objcopy -O ihex -R .eeprom ./bin/EquitorialMountDriver_.elf ./bin/EquitorialMountDriver_.hex

avrdude -q -V -p atmega328p -C /etc/avrdude.conf -D -c arduino -b 115200 -P $PORT -U flash:w:./bin/EquitorialMountDriver_.hex:i
