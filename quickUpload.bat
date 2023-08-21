PORT=com3

./Arduino/hardware/tools/avr//bin/avr-g++\
    -MMD -c -D__PROG_TYPES_COMPAT__ -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=1819\
    -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR -DARDUINO_BOARD=AVR_NANO -DARDUINO_VARIANT=eightanaloginputs
    -I Arduino/hardware/arduino/avr/cores/arduino\
    -I Arduino/hardware/arduino/avr/cores/arduino/api\
    -I Arduino/hardware/arduino/avr/variants/eightanaloginputs\
    -I Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src\
    -Wall -ffunction-sections -fdata-sections -Os -pedantic -Wall -Wextra -fpermissive -fno-exceptions\
    -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color=always main.cpp\
    -o bin/main.cpp.o


"%~dp0Arduino/hardware/tools/avr//bin/avr-gcc" -flto -fuse-linker-plugin -mmcu=atmega328p^
    -Wl,--gc-sections -Os^
    -o "%~dp0bin/EquitorialMountDriver_.elf"^
    "%~dp0/bin/main.cpp.o"^
    "%~dp0/bin/libcore.a" -lc -lm


"%~dp0Arduino/hardware/tools/avr//bin/avr-objcopy" -O ihex -R .eeprom^
    "%~dp0/bin/EquitorialMountDriver_.elf"^
    "%~dp0/bin/EquitorialMountDriver_.hex"


"%~dp0Arduino/hardware/tools/avr/bin/avrdude.exe" -q -V -p atmega328p^
    -C "%~dp0/Arduino/hardware/tools/avr//etc/avrdude.conf"^
    -D -c arduino -b 115200 -P %PORT%^
    -U flash:w:"%~dp0/bin/EquitorialMountDriver_.hex":i"%
