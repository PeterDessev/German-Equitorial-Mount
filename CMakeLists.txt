cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE "cmake-avr-toolchain/avr-gcc.toolchain.cmake")

set(ARDUINO_AVR_DIRECTORY "/c/Program\ Files\ (x86)/Arduino/hardware/arduino/avr")

project(GermanEquitorialMount C CXX ASM)

include_directories(
    "include/"
    "${ARDUINO_AVR_DIRECTORY}/cores/arduino/"
    "${ARDUINO_AVR_DIRECTORY}/cores/arduino/api/"
    "${ARDUINO_AVR_DIRECTORY}/variants/eightanaloginputs/"
    "${ARDUINO_AVR_DIRECTORY}/libraries/SoftwareSerial/src/"
    "${ARDUINO_AVR_DIRECTORY}/libraries/SPI/src/"
)

add_definitions(-DF_CPU=16000000)
set(libraries ${CMAKE_SOURCE_DIR}/bin/core.a ${CMAKE_SOURCE_DIR}/bin/SoftwareSerial.cpp.o)
add_avr_executable(${PROJECT_NAME} "atmega328p" ${libraries}
    src/main.cpp
    src/uart.cpp
)
