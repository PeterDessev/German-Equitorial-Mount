# GEM Driver

An (almost) fully open source German Equitorial Mount currently under development. Publishing of step files for the GEM is under way. The project is based on the Arduino Nano and uses a TMC2209 stepper driver. Amost all parts are either fully 3D-Printable or standard hardware such as metric screws and steel rods.

## Building

Before building initialize and update git submodules, if not already done, with

```zsh
git submodule init
git submodule update
```

Bulding has been tested on Arch linux. To build, run the following.

```zsh
bash quickUpload.sh PORT
```

Where PORT is the arudino's UART port. On linux this is usually `/dev/ttyUSB#`, on windows `com#`.

## TMC Stepper

The main driver of the GEM is a TMC2209 configured over UART. The [TMCStepper library](https://github.com/teemuatlut/TMCStepper/tree/master) was used in the development of the driver, however was stripped down and modified to be more compatable with pure AVR-C, and less reliant on Arduino and the Arduino library. In the future, I would like to completely remove all dependancy on the Arduino Library to simplify building and development.

## Schematic

The schematic was made and can be edited using [KiCad](https://www.kicad.org/). A PCB does not exist yet but is in the works

## Mechanics

Currently the driver is mechanically driven with a NEMA 17 stepper motor mounted on a 100:1 gearbox, though this will be reduced to at least a 25:1 in the future. A 2GT timing belt and two timing pulleys are also used. While printing the pulleys is possible, it is recomended to purchase these to reduce the eccentricity in the drive mechanism, which leads to poor tracking.

### The Worm

The GEM requires a worm gear to drive the tracking mechanism. Printing a worm gear is "possible" depending on the quality of the printer used, however, in most cases it is both easier and produces better results to use a purchased brass worm. In either case, the worm should have a modulus of 0.5 and a bore of at least 6mm to prevent accedentally bending the shaft during assembly or usage. Such worms can be found relativly easily on the internet for around $15, and sometimes come with an accompanying worm drive, which most likely will not be of much use.

## Optics

The GEM also requires a polar scope, which is friction fitted into the gem. The polar scope is the same one used in the SkyWatcher EQ-5, and probably many other mounts, for which replacement parts exist in the usual places on the internet, usually under $100 (I purchased mine off of Amazon for ~$35). Though the polar scope may not be strictly nescessary, since the GEM is much more likely to be limited by the accuracy of the drive mechanism, it makes polar alignment significantly easier, and removes one step in debugging tracking problems.

## Next Steps

Development of the project is planned to continue. Here are the major tasks that still need to be completed, in no particular order:

 - [x] Release CAD files
 - [ ] Move from Atmega to STM32
 - [ ] Create PCB
 - [ ] Add BOM
 - [ ] Create Assembly Instructions
 - [x] Remove Arduino dependancy
 - [x] Release electronics schematic
 - [x] Clean project repo
 - [x] Migrate to a real build system
