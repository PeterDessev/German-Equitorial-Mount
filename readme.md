# GEM Driver

A quick and dirty firmware for driving a german equitorial mount currently under development. A schematic for the electronics and publishing of .step files for the rest of the design are under way.

## TMC Stepper

The main driver of the GEM is a TMC2209 configured over UART. The [TMCStepper library](https://github.com/teemuatlut/TMCStepper/tree/master) was used in the development of the driver, however was stripped down and modified to be more compatable with pure AVR-C, and less reliant on Arduino and their IDE. In the future, I would like to completely remove all traces of the Arduino Library to simplify building and development.

## Building

At the current moment building is a rather tricky endevour which requires a lot of platform specific instructions and even more luck. If you want hints to build this for yourself (for whatever reason), bellow is a ___very___ loose set of instructions to help guide you.

1. Download the OLD (Version 1.x) Arduino IDE
2. Enable verbose output and build a sketch including the software serial header
3. Copy the SoftwareSerial and core (core can be named several things but usually contains the word core) object files to ./bin
4. Copy the Arduino folder, the one which is included in most compilations in the IDE, from the IDE output to the project root. 
4. Run quickBuild.sh or .bash

At the current moment, before a proper build system is implemented, I cannot recomend anyone try to build this. Please be patient, I am working on the problem as fast as I can.

While a .bat file is included, I haven't tested it on Windows with the current way things are so Windows users are on their own.  

## Next Steps

Both continual development of the driver and houskeeping tasks are planned for this project. Here are some of the major tasks I am planning to do, in no particular order:

 - [ ] Release electronics schematic
 - [ ] Release CAD files
 - [x] Migrate to a real build system
 - [ ] Move from Atmega to STM32
 - [ ] Clean project repo