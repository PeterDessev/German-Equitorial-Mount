# GEM Driver

A work in progress firmware for driving a german equitorial mount currently under development. A schematic for the electronics and publishing of .step files for the rest of the design are under way. The project is based on the Arduino Nano, but will be migrated to the STM32 in the future.

## TMC Stepper

The main driver of the GEM is a TMC2209 configured over UART. The [TMCStepper library](https://github.com/teemuatlut/TMCStepper/tree/master) was used in the development of the driver, however was stripped down and modified to be more compatable with pure AVR-C.

## Building

Before building initialize and update git submodules, if not already done, with the following.

```zsh
git submodule init
git submodule update
```

Bulding has been tested on Arch linux. To build, run the following.

```zsh
bash quickUpload.sh PORT
```

Where PORT is the arudino's UART port. On linux this is usually `/dev/ttyUSB#`, on windows `com#`.

# Schematic

The schematic was made and can be edited using (KiCad)[https://www.kicad.org/]. A PCB does not exist yet but is in the works

## Next Steps

Both continual development of the driver and houskeeping tasks are planned for this project. Here are some of the major tasks I am planning to do, in no particular order:

 - [ ] Release CAD files
 - [ ] Move from Atmega to STM32
 - [ ] Create PCB
 - [x] Remove Arduino dependancy
 - [x] Release electronics schematic
 - [x] Clean project repo
 - [x] Migrate to a real build system
