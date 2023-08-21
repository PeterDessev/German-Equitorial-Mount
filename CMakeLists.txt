cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE "cmake-avr-toolchain/avr-gcc.toolchain.cmake")

project(GermanEquitorialMount C CXX ASM)

include_directories(
    include/
    Arduino/hardware/arduino/avr/cores/arduino/
    Arduino/hardware/arduino/avr/cores/arduino/api/
    Arduino/hardware/arduino/avr/variants/eightanaloginputs/
    Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/
    Arduino/hardware/arduino/avr/libraries/SPI/src/
)

add_definitions(-DF_CPU=16000000)
set(libraries ${CMAKE_SOURCE_DIR}/bin/core.a ${CMAKE_SOURCE_DIR}/bin/SoftwareSerial.cpp.o)
add_avr_executable(${PROJECT_NAME} "atmega328p" ${libraries}
    src/main.cpp
    src/uart.cpp
)
