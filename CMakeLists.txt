cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE "cmake-avr-toolchain/avr-gcc.toolchain.cmake")

project(GermanEquitorialMount C ASM)

include_directories(
    "include/"
)

add_definitions(-DF_CPU=16000000)
add_avr_executable(${PROJECT_NAME} "atmega328p"
    src/main.c
    src/tmc2209.c
    src/hwUart.c
    src/softwareUart.c
)
