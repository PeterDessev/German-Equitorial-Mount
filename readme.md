# GEM Driver

A quick and dirty firmware for driving a german equitorial mount currently under development. A schematic for the electronics and publishing of .step files for the rest of the design are under way. The project is based on the Arduino Nano.

## TMC Stepper

The main driver of the GEM is a TMC2209 configured over UART. The [TMCStepper library](https://github.com/teemuatlut/TMCStepper/tree/master) was used in the development of the driver, however was stripped down and modified to be more compatable with pure AVR-C, and less reliant on Arduino and their IDE. In the future, I would like to completely remove all traces of the Arduino Library to simplify building and development.

## Building

Before building initialize and update git submodules, if not already done, with

```zsh
git submodule init
git submodule update
```

Bulding has been tested on Arch linux. In order to build first modify `ARDUINO_AVR_DIRECTORY` in `CMakeLists.txt` to the path of the `avr` folder in an Arduino installation. Then run:

```zsh
bash quickUpload.sh PORT
```

Where PORT is the arudino's UART port.

# Schematic

The schematic was made and can be edited using (KiCad)[https://www.kicad.org/]. A PCB does not exist yet but is in the works

## Next Steps

Both continual development of the driver and houskeeping tasks are planned for this project. Here are some of the major tasks I am planning to do, in no particular order:

 - [ ] Release CAD files
 - [ ] Move from Atmega to STM32
 - [ ] Remove Arduino dependancy
 - [ ] Create PCB
 - [x] Release electronics schematic
 - [x] Clean project repo
 - [x] Migrate to a real build system
